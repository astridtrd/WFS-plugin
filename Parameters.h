/*
  ==============================================================================

    Parameters.h
    Created: 28 Oct 2024 12:37:30pm
    Author:  agonot

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SpeakerConfigData.h"
#include "SpeakerSourceDisplay.h"

#define MAX_DISTANCE 100.0f
// 48 HPs L config
/*
#define MIN_SOURCE_X 0.0f
#define MAX_SOURCE_X 9.0f
#define MIN_SOURCE_Z -4.5f
#define MAX_SOURCE_Z 4.5f

#define MIN_LISTENER_X 0.0f
#define MAX_LISTENER_X 4.5f
#define MIN_LISTENER_Z -4.5f
#define MAX_LISTENER_Z 0.0f
*/

// 16 HPs Linear config
#define MIN_SOURCE_X -2.0f
#define MAX_SOURCE_X 2.0f
#define MIN_SOURCE_Z -2.0f
#define MAX_SOURCE_Z 2.0f

#define MIN_LISTENER_X -2.0f
#define MAX_LISTENER_X 2.0f
#define MIN_LISTENER_Z -2.0f
#define MAX_LISTENER_Z 0.0f

const juce::ParameterID sourcePosXParamID{ "sourcePosX", 1 };
const juce::ParameterID sourcePosZParamID{ "sourcePosZ", 1 };

const juce::ParameterID listenerPosXParamID{ "listenerPosX", 1 };
const juce::ParameterID listenerPosZParamID{ "listenerPosZ", 1 };

const juce::ParameterID sourceRotationParamID{ "sourceRotation", 1 };
const juce::ParameterID bypassGainParamID{ "bypassGain", 1 };
const juce::ParameterID bypassPHParamID{ "bypassPH", 1 };

class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts, float * speakerMask, int numOutput);
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();
    ~Parameters();

    void update() noexcept;
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    void smoothGains(juce::Array<float>& gainArray, int windowSize);
    juce::Array<float> wfsGainAtt();
    float filtrePasseHaut(float inSamplef);

    // bypass
    int _bypassGain;
    int ph;

    // current source position
    float _sourcePosX = 0.0f;
    float _sourcePosZ = 1.0f;
    // current source rotation
    float _sourceRotation = 0.0f;
    // current listener position
    float _listenerPosX = 0.0f;
    float _listenerPosZ = -1.0f;


    // array of delay value apply on each channel
    // /!\ smoothing should be applied !! TODO
    float* _pWfsDelay = nullptr;
    // min and max delay in the previous array use to apply time 
    // reversal for focused source (inside the listening area)
    float _wfsMinDelay, _wfsMaxDelay;

    // pointer to the speaker mask define in the audio processor.
    // The array is updated in the smoothen() method, when the position/angle 
    // of the source is updated
    // /!\ smoothing should be applied !! TODO
    float* _speakerMask = nullptr;
    // pointer to the speaker layout define in the audio processor given the speaker position and orientation
    LoudspeakerLayout* _loudspeakerLayout = nullptr;
    // pointer to the component in charge of the display
    SpeakerSourceDisplay *_pSpeakerSourceDisplay = nullptr;

    int _numOutput;

private:

    bool _isInside;

    juce::LinearSmoothedValue<float> _sourcePosXSmoother;
    juce::AudioParameterFloat* _sourcePosXParam;

    juce::LinearSmoothedValue<float> _sourcePosZSmoother;
    juce::AudioParameterFloat* _sourcePosZParam;

    juce::LinearSmoothedValue<float> _listenerPosXSmoother;
    juce::AudioParameterFloat* _listenerPosXParam;

    juce::LinearSmoothedValue<float> _listenerPosZSmoother;
    juce::AudioParameterFloat* _listenerPosZParam;

    juce::LinearSmoothedValue<float> _sourceRotationSmoother;
    juce::AudioParameterFloat* _sourceRotationParam;

    juce::AudioParameterFloat* _bypassGainParam;
    juce::AudioParameterFloat* _bypassPHParam;
    
    float b0 = 1.0f, b1 = -1.0f, b2 = 0.0f, a0 = 1.0f, a1 = -0.876976462992756888858991715096635743976f, a2 = 0.0f;

    // method used to update the _speakerMask array
    void updateSpeakerMask();

    // method used to update the _pWfsDelay array
    void udpateWfsDelay();
};
