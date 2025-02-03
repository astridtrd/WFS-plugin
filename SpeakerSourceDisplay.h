/*
  ==============================================================================

    SpeakerSourceDisplay.h
    Created: 7 Dec 2024 4:04:56pm
    Author:  agonot

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SpeakerConfigData.h"


//==============================================================================
/*
*/
class SpeakerSourceDisplay  : public juce::Component, private juce::Timer
{
public:
    SpeakerSourceDisplay();
    ~SpeakerSourceDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
   
    // pointer to the speaker layout define in the audio processor given the speaker position and orientation
    LoudspeakerLayout *_loudspeakerLayout = nullptr;
    // pointer to the speaker mask define in the audio processor class and updated in the Parameters class
    float* _speakerMask = nullptr;

    // Call by the Parameters class to udpate source and listener position/angle
    void setSourceListenerPosition(float sourcePosX, float sourcePosZ, 
        float listenerPosX, float listenerPosZ, float sourceRotation);

private:

    // Timer callbakc used to update the display (interval) - cause the call of the paint() method     
    // /!\ If no timer is used the paint() method is only called at startup and when the editor of the 
    // plugin is moved by the user
    // /!\ Bad method => should use OpenGL rendering instead to save CPU
    void timerCallback() final {
        repaint();
    };

    float _sourcePosX = 0, _sourcePosZ = 0, _listenerPosX = 0, _listenerPosZ = 0, _sourceRotation = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpeakerSourceDisplay)
};
