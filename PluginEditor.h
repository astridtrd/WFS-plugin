/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "Knob.h"
#include "SpeakerSourceDisplay.h"

//==============================================================================
/**
*/
class PFE_WFS_simpleAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PFE_WFS_simpleAudioProcessorEditor (PFE_WFS_simpleAudioProcessor& p, Parameters* params);
    ~PFE_WFS_simpleAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    PFE_WFS_simpleAudioProcessor& audioProcessor;

private:

    Parameters* params;
    
    // Editor groups
    juce::GroupComponent _sourceGroup, _listenerGroup, _speakerSourceDisplayGroup, _filterGroup;

    // Editor knobs 
    Knob _sourcePosX_Knob{ "Position X", audioProcessor.apvts, sourcePosXParamID, juce::Slider::SliderStyle::LinearHorizontal };
    Knob _sourcePosZ_Knob{ "Position Z", audioProcessor.apvts, sourcePosZParamID, juce::Slider::SliderStyle::LinearHorizontal };

    Knob _listenerPosX_Knob{ "Position X", audioProcessor.apvts, listenerPosXParamID, juce::Slider::SliderStyle::LinearHorizontal };
    Knob _listenerPosZ_Knob{ "Position Z", audioProcessor.apvts, listenerPosZParamID, juce::Slider::SliderStyle::LinearHorizontal };
    
    Knob _sourceRotation_Knob{ "Rotation", audioProcessor.apvts, sourceRotationParamID, juce::Slider::SliderStyle::LinearHorizontal };

    Knob _bypassGain_Knob{ "gain/att", audioProcessor.apvts, bypassGainParamID, juce::Slider::SliderStyle::LinearHorizontal };
    Knob _bypassPH_Knob{ "passe-haut", audioProcessor.apvts, bypassPHParamID, juce::Slider::SliderStyle::LinearHorizontal };

    // Editor component for displaying speakers, source and listener
    SpeakerSourceDisplay _speakerSourceDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PFE_WFS_simpleAudioProcessorEditor)
};
