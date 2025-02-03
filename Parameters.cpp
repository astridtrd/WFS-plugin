/*
  ==============================================================================

    Parameters.cpp
    Created: 28 Oct 2024 12:37:30pm
    Author:  agonot

  ==============================================================================
*/

#include "Parameters.h"
#include <cmath>
#include "parameters.h"
#define PI 3.1415926535897

static juce::String stringFromOnOff(int value, int)
{
    return juce::String((float)value, 1);
}

static juce::String stringFromPosition(float value, int)
{
    return juce::String(value, 1);
}
static juce::String stringFromRotation(float value, int)
{
    return juce::String(value, 1) + "°";
}

//==============================================================================
template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination); // parameter does not exist or wrong type
}
//==============================================================================
Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts, float* speakerMask, int numOutput)
{   
    _speakerMask = speakerMask;
    _numOutput = numOutput;
    _pWfsDelay = new float[numOutput];

    castParameter<juce::AudioParameterFloat*>(apvts, sourcePosXParamID, _sourcePosXParam);
    castParameter<juce::AudioParameterFloat*>(apvts, sourcePosZParamID, _sourcePosZParam);

    castParameter<juce::AudioParameterFloat*>(apvts, listenerPosXParamID, _listenerPosXParam);
    castParameter<juce::AudioParameterFloat*>(apvts, listenerPosZParamID, _listenerPosZParam);

    castParameter<juce::AudioParameterFloat*>(apvts, sourceRotationParamID, _sourceRotationParam);
    castParameter<juce::AudioParameterFloat*>(apvts, bypassGainParamID, _bypassGainParam);
    castParameter<juce::AudioParameterFloat*>(apvts, bypassPHParamID, _bypassPHParam);
}

Parameters::~Parameters() {
    delete[] _pWfsDelay;
}
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        sourcePosXParamID,
        "Position X",
        juce::NormalisableRange<float> {MIN_SOURCE_X, MAX_SOURCE_X },
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPosition)
    ));
   
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        sourcePosZParamID,
        "Position Z",
        juce::NormalisableRange<float> {MIN_SOURCE_Z, MAX_SOURCE_Z},
        1.0,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPosition)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        listenerPosXParamID,
        "Position X",
        juce::NormalisableRange<float> {MIN_LISTENER_X, MAX_LISTENER_Z },
        0,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPosition)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        listenerPosZParamID,
        "Position Z",
        juce::NormalisableRange<float> {MIN_LISTENER_Z, MAX_LISTENER_Z},
        MIN_LISTENER_Z/2.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPosition)
    ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        sourceRotationParamID,
        "Rotation",
        juce::NormalisableRange<float> {0, 360},
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromRotation)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        bypassGainParamID,
        "Bypass",
        juce::NormalisableRange<float> {0,1},
        0,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromOnOff)

   ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        bypassPHParamID,
        "Filtre PH",
        juce::NormalisableRange<float> {0, 1},
        0,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromOnOff)

    ));


    return layout;
}


//==============================================================================
void Parameters::update() noexcept {

    _sourcePosXSmoother.setTargetValue(_sourcePosXParam->get());
    _sourcePosZSmoother.setTargetValue(_sourcePosZParam->get());

    _listenerPosXSmoother.setTargetValue(_listenerPosXParam->get());
    _listenerPosZSmoother.setTargetValue(_listenerPosZParam->get());

    _sourceRotationSmoother.setTargetValue(_sourceRotationParam->get());

    _bypassGain = (int)_bypassGainParam->get();
    ph = (int)_bypassPHParam->get();
    

    

}

//==============================================================================
void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    _sourcePosXSmoother.reset(sampleRate, duration);
    _sourcePosZSmoother.reset(sampleRate, duration);

    _listenerPosXSmoother.reset(sampleRate, duration);
    _listenerPosZSmoother.reset(sampleRate, duration);

    _sourceRotationSmoother.reset(sampleRate, duration);
}
//==============================================================================
void Parameters::reset() noexcept
{
    _sourcePosXSmoother.setCurrentAndTargetValue(_sourcePosXParam->get());
    _sourcePosZSmoother.setCurrentAndTargetValue(_sourcePosZParam->get());

    _listenerPosXSmoother.setCurrentAndTargetValue(_listenerPosXParam->get());
    _listenerPosZSmoother.setCurrentAndTargetValue(_listenerPosZParam->get());

    _sourceRotationSmoother.setCurrentAndTargetValue(_sourceRotationParam->get());
}

void Parameters::updateSpeakerMask() {

    int positiveDotCount = 0;
    // temporary array used to store the speaker mask when the source is outside 
    // the listening area (behind the speakers)
    float* outsideMask = new float[_numOutput];

    
    if (_speakerMask != nullptr && _loudspeakerLayout != nullptr) {
         
        // ---------------------------------------------------
        // Test if the source is in the listening area
        // ---------------------------------------------------
        for (int i = 0; i < _numOutput; i++) {
            float spkX = _loudspeakerLayout->_loudspeakers[i]._posX;
            float spkZ = _loudspeakerLayout->_loudspeakers[i]._posZ;
            float fwdAngle = _loudspeakerLayout->_loudspeakers[i]._fwdAngle;

            float spkSourceVectorX = spkX - _sourcePosX;
            float spkSourceVectorZ = spkZ - _sourcePosZ;

            // Store the value for the speaker mask, in case the source is outside the listening area
            // Get the dot product of the Speaker-Source vector and the speaker angle
            float dotSource = spkSourceVectorX * sinf(fwdAngle * PI / 180.0f) + spkSourceVectorZ * cosf(fwdAngle * PI / 180.0f);
            // if the dot product is less than 0, the speaker is ON when the source is outside
            if (dotSource >= 0) {
                outsideMask[i] = 1.0f;
            }
            // if the dot product is greater than 0, the speaker is OFF when the source is outside
            else {
                outsideMask[i] = 0.0f;
                // count the number of positive dot product. 
                // If all are positive, the source is inside the speaker setup
                positiveDotCount++;

            }
        }
        
        // ---------------------------------------------------
        // Set the speaker mask applied for display and audio processing
        // ---------------------------------------------------     
        for (int i = 0; i < _numOutput; i++) {

            // If the source is inside the listening area    
            if (positiveDotCount == _numOutput) {
                _isInside = true;
                float spkX = _loudspeakerLayout->_loudspeakers[i]._posX;
                float spkZ = _loudspeakerLayout->_loudspeakers[i]._posZ;
                float fwdAngle = _loudspeakerLayout->_loudspeakers[i]._fwdAngle;

                float spkSourceVectorX = _sourcePosX - spkX;
                float spkSourceVectorZ = _sourcePosZ - spkZ;

                // Get the Dot product of the Speaker-Source vector and the forward vector of the souce (from rotation angle)
                float dotSource = spkSourceVectorX * sinf(_sourceRotation * PI / 180.0f) + spkSourceVectorZ * cosf(_sourceRotation * PI / 180.0f);
                // if the dot product is greater than 0, the speaker "is behind" the source, so it is ON
                if (dotSource > 0) {
                    _speakerMask[i] = 1.0f;
                }
                // if the dot product is less than 0, the speaker is "in front" the source, so it is OFF
                else {
                    
                    _speakerMask[i] = 0.0f; //DE BASE = 0 !!!! 
                }                
            }
            // If the source is outside the listening area
            // We use the previous mask 
            else {
                _isInside = false;
                _speakerMask[i] = outsideMask[i];
            }
        }
   
    }

    delete[] outsideMask;

}

void Parameters::udpateWfsDelay() {

    if (_loudspeakerLayout != nullptr && _pWfsDelay != nullptr) {

        _wfsMinDelay = 0x7f7fffff; // float max value
        _wfsMaxDelay = 0.0f;

        for (int i = 0; i < _numOutput; i++) {
            float direction[2];            
            static const float kRad2Deg = 180.0f / PI;
            direction[0] = _sourcePosX - _loudspeakerLayout->_loudspeakers[i]._posX;
            direction[1] = _sourcePosZ - _loudspeakerLayout->_loudspeakers[i]._posZ;
            float sourceSpeakerDistance = sqrtf(pow(direction[0], 2) + pow(direction[1], 2));            
            
            // set the delay for this output channel
            _pWfsDelay[i] = sourceSpeakerDistance / 340.0f;

            // update the min/ max values
            if (_pWfsDelay[i] > _wfsMaxDelay)
                _wfsMaxDelay = _pWfsDelay[i];
            if (_pWfsDelay[i] < _wfsMinDelay)
                _wfsMinDelay = _pWfsDelay[i];
        }

        // If inside, we apply time reversal for focused source
        if (_isInside) {
             for (int i = 0; i < _numOutput; i++) {
                _pWfsDelay[i] = _wfsMaxDelay + _wfsMinDelay - _pWfsDelay[i];
            }
        }
    }
    
}

//==============================================================================
void Parameters::smoothen() noexcept
{
    _sourcePosX = _sourcePosXSmoother.getNextValue();
    _sourcePosZ = _sourcePosZSmoother.getNextValue();  

    _listenerPosX = _listenerPosXSmoother.getNextValue();
    _listenerPosZ = _listenerPosZSmoother.getNextValue();

    _sourceRotation = _sourceRotationSmoother.getNextValue();

    updateSpeakerMask();

    udpateWfsDelay();

    if (_pSpeakerSourceDisplay != nullptr)
        _pSpeakerSourceDisplay->setSourceListenerPosition(_sourcePosX, _sourcePosZ, _listenerPosX, _listenerPosZ, _sourceRotation);

}




//=================GAIN ET ATTENUATION=================

//===== Fonction pour lisser le tableau de gain =========

void Parameters::smoothGains(juce::Array<float>& gainArray, int windowSize)
{
    // Vérification que la taille de la fenêtre est valide
    if (windowSize <= 0 || windowSize > gainArray.size())
        return;

    juce::Array<float> smoothedArray(gainArray);

    // Lissage des valeurs avec la moyenne des voisins
    for (int i = 0; i < gainArray.size(); ++i)
    {
        float sum = 0.0f;
        int count = 0;

        // Calcul de la somme des voisins dans la fenêtre
        for (int j = -windowSize / 2; j <= windowSize / 2; ++j)
        {
            int index = i + j;

            if (index >= 0 && index < gainArray.size())
            {
                sum += gainArray[index];
                count++;
            }
        }

        // Calcul de la moyenne et mise à jour de la valeur lissée
        smoothedArray.set(i, sum / count);
    }

    // Copier les valeurs lissées dans le tableau d'origine
    gainArray = smoothedArray;
}


//======= Calcul de l'attenuation et du gain =========

juce::Array<float> Parameters::wfsGainAtt() {
    juce::Array<float> gainArray; 
    float gain;
    float attenuation;
    for (int i = 0; i < _numOutput; i++) {
        float direction[2];
        direction[0] = abs(_sourcePosX - _loudspeakerLayout->_loudspeakers[i]._posX);
        direction[1] = abs(_sourcePosZ - _loudspeakerLayout->_loudspeakers[i]._posZ);
        float sourceSpeakerDistance = sqrtf(pow(direction[0], 2) + pow(direction[1], 2));

      
        gain = (abs(cos(_loudspeakerLayout->_loudspeakers[i]._fwdAngle)) / (2 * PI * pow(sourceSpeakerDistance, 1 / 2)));
        //float g2 = (abs(_sourcePosZ) / (2 * PI * pow(sourceSpeakerDistance, 3 / 2)));

        //gain = 1;
/*
        // Affichage des valeurs de direction et de distance
        DBG("Speaker " + juce::String(i) + " - Direction X: " + juce::String(direction[0]) +
            ", Direction Z: " + juce::String(direction[1]) +
            ", Distance: " + juce::String(sourceSpeakerDistance));
*/
        // CAS SOURCE VIRTUELLE 
        if (_sourcePosZ > 0) {
            attenuation  = sqrtf(abs(_listenerPosZ) / (abs(_listenerPosZ - _sourcePosZ)));
        }

        // CAS SOURCE FOCALISEE 
        else if (_sourcePosZ <= 0) {
            attenuation = 1 / (1 + abs(_listenerPosZ - _sourcePosZ)); // A MODIFIER PEUT ETRE QUAND ON TEST
        }

        // Affichage des valeurs de gain et d'atténuation
        //DBG("Gain: " + juce::String(gain) + ", Attenuation: " + juce::String(attenuation));

        gainArray.add(gain * attenuation);
        float ga = (gain * attenuation);
    }

    // Appliquer le lissage des gains
    smoothGains(gainArray, 3);  // Utilise une fenêtre de taille 3

    return gainArray; 
}

float Parameters::filtrePasseHaut(float inSamplef)
{
    float in_z1, in_z2, out_z0, out_z1, out_z2, out_z3;  
    float outSamplef = a0 * inSamplef + a1 * in_z1 + a2 * in_z2 - (b0 * out_z1 + b1 * out_z2  + b2 * out_z3);
    in_z2 = in_z1;
    in_z1 = outSamplef; 
    out_z3 = out_z2;
    out_z2 = out_z1;
    out_z1 = inSamplef;

    return outSamplef;
}



