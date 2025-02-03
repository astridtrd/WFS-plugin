/*
  ==============================================================================

    SpeakerSourceDisplay.cpp
    Created: 7 Dec 2024 4:04:56pm
    Author:  agonot

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpeakerSourceDisplay.h"
#include <cmath>
#define PI 3.1415926535897
//==============================================================================
SpeakerSourceDisplay::SpeakerSourceDisplay()
{
    // set the time interval of the timer in milliseconds
    startTimer(100.0f);
}

SpeakerSourceDisplay::~SpeakerSourceDisplay()
{
}

void SpeakerSourceDisplay::setSourceListenerPosition(float sourcePosX, float sourcePosZ, 
    float listenerPosX, float listenerPosZ, float sourceRotation) {
    _sourcePosX = sourcePosX;
    _sourcePosZ = sourcePosZ;
    _listenerPosX = listenerPosX;
    _listenerPosZ = listenerPosZ;
    _sourceRotation = sourceRotation;
}

void SpeakerSourceDisplay::paint (juce::Graphics& g)
{

    float speakerSize = 5.0f;
    float sourceSize = 10.0f;
    float listenerSize = 10.0f;

    g.fillAll (juce::Colours::black);   // clear the background
   
    float width = getWidth();
    float height = getHeight();

    if (_loudspeakerLayout != nullptr && _speakerMask != nullptr) {

        float realWidthRatio = width / (_loudspeakerLayout->_maxX - _loudspeakerLayout->_minX);
        float realheightRatio = height / (_loudspeakerLayout->_maxZ - _loudspeakerLayout->_minZ);

        // -------------------------------------------------------------------------------------
        // Draw speakers
        // -------------------------------------------------------------------------------------
        int numSpk = _loudspeakerLayout->_loudspeakers.size();

        for (int i = 0; i < numSpk; i++) {
            Loudspeakers s = _loudspeakerLayout->_loudspeakers[i];
            float screenPosX = width/2.0f + s._posX * realWidthRatio;
            float screenPosY = height / 2.0f - s._posZ * realheightRatio;

            juce::Rectangle<int> spkRect(screenPosX - speakerSize / 2.0f, screenPosY - speakerSize / 2.0f, speakerSize, speakerSize);
            g.setColour(juce::Colour(255.0f, 255.0f * _speakerMask[i], 255.0f * _speakerMask[i]));
            g.fillRect(spkRect);
        }

        // -------------------------------------------------------------------------------------
        // Draw listener
        // -------------------------------------------------------------------------------------
        juce::Path listenerPath;
        float listenerScreenPosX = width / 2.0f + _listenerPosX * realWidthRatio;
        float listenerScreenPosZ = height / 2.0f - _listenerPosZ * realheightRatio;
        listenerPath.addEllipse(listenerScreenPosX - listenerSize / 2.0f, listenerScreenPosZ - listenerSize / 2.0f, listenerSize, listenerSize);
        g.setColour(juce::Colours::blueviolet);
        g.fillPath(listenerPath);
        g.setFont(12.0f);
        g.drawText("listener", listenerScreenPosX, listenerScreenPosZ, 40, 20, juce::Justification::centred, true);

        // -------------------------------------------------------------------------------------
        // Draw source
        // -------------------------------------------------------------------------------------
        juce::Path sourcePath;
        float sourceScreenPosX = width / 2.0f + _sourcePosX * realWidthRatio;
        float sourceScreenPosZ = height / 2.0f - _sourcePosZ * realheightRatio;

        float angle = _sourceRotation - 90.0f;
        juce::Point<float> forward(sourceScreenPosX + sourceSize * cosf(angle * PI / 180.0f),
            sourceScreenPosZ + sourceSize * sinf(angle * PI / 180.0f));

        angle = _sourceRotation + 150.0f;
        juce::Point<float> leftBack(sourceScreenPosX + sourceSize * cosf(angle * PI / 180.0f),
            sourceScreenPosZ + sourceSize * sinf(angle * PI / 180.0f));

        angle = _sourceRotation + 30.0f;
        juce::Point<float> rightBack(sourceScreenPosX + sourceSize * cosf(angle * PI / 180.0f),
            sourceScreenPosZ + sourceSize * sinf(angle * PI / 180.0f));

        sourcePath.addTriangle(forward, rightBack, leftBack);
        g.setColour(juce::Colours::green);
        g.fillPath(sourcePath);

        juce::Path sourceForwardPath;
        sourceForwardPath.addEllipse(forward.x - sourceSize / 4.0f, forward.y - sourceSize / 4.0f,
            sourceSize / 2.0f, sourceSize / 2.0f);
        g.fillPath(sourceForwardPath);

        g.setFont(12.0f);
        g.drawText("Source", sourceScreenPosX + sourceSize / 4.0f, sourceScreenPosZ + sourceSize / 4.0f, 40, 20, juce::Justification::centred, true);
    }

}

void SpeakerSourceDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
