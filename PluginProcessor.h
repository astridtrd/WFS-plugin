/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SpeakerConfigData.h"
#include "Parameters.h"

//#define NUM_OUTPUT 48
#define NUM_OUTPUT 16

//==============================================================================
/**
*/
class PFE_WFS_simpleAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PFE_WFS_simpleAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts{
       *this, nullptr, "Parameters", Parameters::createParameterLayout()
    };

    Parameters params;

    // array of float given the mask for the speakers : 0 = speaker off / 1 = speaker on
    float _speakerMask[NUM_OUTPUT];

    // Number of channels/speakers for WFS
    int _numOutputChannel = NUM_OUTPUT;

    // See definitions in "SpeakerConfigData.h" ---------------------------------------------
    // Array of float array (x4) given the position and angle of each speaker :
    // Index 0 => speaker X position     
    // Index 1 => speaker Y position
    // Index 2 => speaker Z position
    // Index 3 => angle (in degree) of the
    
    //std::vector<std::vector<float>> _loudspeakers{ LoudspeakerLayout::calcSpkConfif48L() };
    std::vector<std::vector<float>> _loudspeakers{ LoudspeakerLayout::calcSpkConfif16lin()};

    
    // Structure storing the name of the speaker layout, the speaker position/angle list, and the X/Z min/max for 
    // adapting the display (and the conversion from real to screen coordinates) 
    LoudspeakerLayout _loudspeakerLayout_config48L{ "PRISM 48 HPs - L config", _loudspeakers, 
        MIN_SOURCE_X, MAX_SOURCE_X, MIN_SOURCE_Z, MAX_SOURCE_Z };

private:
    

    // Delay line storing the history of the audio data
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> _wfsDelayLine;

    // Each DAW has its own channel order depending on the number of channels in the bus (either input or output), 
    // which is different from the channel order in JUCE.
    // The conversion from Juce To Reaper for 48 channels is given by the table below :
    int channelMappingReaper48[48] =
    {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
    47,
    21,22, 23, 24, 25, 26,
    19, 20,
    37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
    27, 28, 29, 30, 31, 32, 33, 34, 35, 36 
    };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PFE_WFS_simpleAudioProcessor)

    float b0 = 1.0f, b1 = -1.0f, b2 = 0.0f, a0 = 1.0f, a1 = -0.876976462992756888858991715096635743976f, a2 = 0.0f;
};
