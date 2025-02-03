/*
  ==============================================================================

    Knob.h
    Created: 28 Oct 2024 12:37:49pm
    Author:  agonot

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Knob : public juce::Component
{
public:

    Knob(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, juce::Slider::SliderStyle style);
    ~Knob() override;

    void resized() override;

    juce::Slider slider;
    juce::Label label;

    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Knob)
};

