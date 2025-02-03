/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PFE_WFS_simpleAudioProcessor::PFE_WFS_simpleAudioProcessor()
     : AudioProcessor (BusesProperties() 
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)                   
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)                    
                       ),
    params(apvts, _speakerMask, NUM_OUTPUT)
{    
    for (int i = 0; i < NUM_OUTPUT; i++) _speakerMask[i] = 0.0f;
    
    params._loudspeakerLayout = &_loudspeakerLayout_config48L;

}

//==============================================================================
const juce::String PFE_WFS_simpleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PFE_WFS_simpleAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PFE_WFS_simpleAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PFE_WFS_simpleAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PFE_WFS_simpleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PFE_WFS_simpleAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PFE_WFS_simpleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PFE_WFS_simpleAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PFE_WFS_simpleAudioProcessor::getProgramName (int index)
{
    return {};
}

void PFE_WFS_simpleAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PFE_WFS_simpleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 1;
    _wfsDelayLine.prepare(spec);

    double numSamples = (MAX_DISTANCE / 340.0f) * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));
    _wfsDelayLine.setMaximumDelayInSamples(maxDelayInSamples);
    _wfsDelayLine.reset();
}

void PFE_WFS_simpleAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PFE_WFS_simpleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return true;
}
#endif



void PFE_WFS_simpleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //Création d'un tableau de gains + gain
    juce::Array<float> gainArray;
    gainArray = params.wfsGainAtt();
    float gain;
   
    // Update the parameters value in the AudioProcessorValueTreeState instance
    params.update();
    float sampleRate = float(getSampleRate());

    // Get the channel 0 for mono input
    float* input = buffer.getWritePointer(0);
        
    // Save the read postion in the ring buffer (delay line)
    // because the read position is incremented at each popSAmple() call (3rd argument == true)
    // => For each channel, the initial value of the read position will be restored
    int intialReadpos = _wfsDelayLine.getReadPos(0);
    
    // Loop on each output channels
    for (int channel = 0; channel < totalNumOutputChannels; channel++) {

        // Get the buffer for the output channel (after conversion to get the right channel in Reaper)
        float *output = buffer.getWritePointer(channelMappingReaper48[channel]);

        // we set the fractionnal delay in sample
        float delaySample = params._pWfsDelay[channel] * sampleRate;

        //gain (channel-ième float du tableau gainArray)
        gain = gainArray[channel];
        
        // loop on each samples for a givent channel
        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
            
            // update smoothing fo parameter and push sample of the input buffer in the delay line
            // Only for the channel 0 !
            if (channel == 0) {
                params.smoothen(); 
                _wfsDelayLine.pushSample(0, input[sample]);
            }
            
            if (params._bypassGain == 1.0) {
                output[sample] = _wfsDelayLine.popSample(0, delaySample, true) * (float)params._speakerMask[channel] * gain;
            }

            else if (params.ph == 1) {
                float in_z1, in_z2, out_z0, out_z1, out_z2, out_z3;
                float inSamplef = _wfsDelayLine.popSample(0, delaySample, true) * (float)params._speakerMask[channel];
                output[sample] = a0 * (inSamplef)+a1 * in_z1 + a2 * in_z2 - (b0 * out_z1 + b1 * out_z2 + b2 * out_z3);
                in_z2 = in_z1;
                in_z1 = output[sample];
                out_z3 = out_z2;
                out_z2 = out_z1;
                out_z1 = inSamplef;
            }
            
            else if (params._bypassGain == 1.0 && params.ph == 1.0) {
                float inSamplef = _wfsDelayLine.popSample(0, delaySample, true) * (float)params._speakerMask[channel] * gain;
                //application du filtre passe haut
                float in_z1, in_z2, out_z0, out_z1, out_z2, out_z3;
                output[sample] =  a0 * inSamplef + a1 * in_z1 + a2 * in_z2 - (b0 * out_z1 + b1 * out_z2 + b2 * out_z3);
                in_z2 = in_z1;
                in_z1 = output[sample];
                out_z3 = out_z2;
                out_z2 = out_z1;
                out_z1 = inSamplef;
            }
            
            
            else {
                output[sample] = (_wfsDelayLine.popSample(0, delaySample, true) * (float)params._speakerMask[channel]);
            }
            

        }
        // Restore the initial value of the read position
        // Not for the last channel : the read position muste updated for the next processBlock() call (nex sample block)
        if (channel < totalNumOutputChannels - 1)
            _wfsDelayLine.setReadPos(0, intialReadpos);
    }
}

//==============================================================================
bool PFE_WFS_simpleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PFE_WFS_simpleAudioProcessor::createEditor()
{
    return new PFE_WFS_simpleAudioProcessorEditor (*this, &params);
}

//==============================================================================
void PFE_WFS_simpleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Serialize the value of all parameters 
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void PFE_WFS_simpleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Deserialize the value of all parameters 
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PFE_WFS_simpleAudioProcessor();
}
