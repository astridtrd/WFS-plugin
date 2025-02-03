 /*
  ==============================================================================

    Knob.cpp
    Created: 28 Oct 2024 12:37:49pm
    Author:  agonot

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Knob.h"

//==============================================================================
Knob::Knob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID, juce::Slider::SliderStyle style)
    : attachment(apvts, parameterID.getParamID(), slider)
{
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 16);
    if (style == juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
        || style == juce::Slider::SliderStyle::RotaryHorizontalDrag
        || style == juce::Slider::SliderStyle::RotaryVerticalDrag
        || style == juce::Slider::SliderStyle::Rotary)
        slider.setBounds(0, 0, 70, 86);
    else if (style == juce::Slider::SliderStyle::LinearHorizontal) {
        slider.setBounds(0, 0, 70, 40);
    }
    addAndMakeVisible(slider);

    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::horizontallyCentred);
    label.setBorderSize(juce::BorderSize<int>{ 0, 0, 2, 0 });
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);
    setSize(70, 110);
}

Knob::~Knob()
{
}

void Knob::resized()
{
    slider.setTopLeftPosition(0, 24);
}
