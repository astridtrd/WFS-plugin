/*
  ==============================================================================

    SpeakerConfigData.h
    Created: 7 Dec 2024 1:06:55pm
    Author:  agonot

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Loudspeakers {
public:
    float _posX; // right
    float _posY; // top
    float _posZ; // front
    float _fwdAngle; 
    int _channel;

    // Constructor (setup)
    Loudspeakers(float channel, float x, float y, float z, float fwdAngle)
        : _channel(channel), _posX(x), _posY(y), _posZ(z), _fwdAngle(fwdAngle)
    {}
};

class LoudspeakerLayout {
public:
    juce::String _name;
    float _minX, _maxX, _minZ, _maxZ; 
    std::vector<Loudspeakers> _loudspeakers;

    // Constructor (setup)
    LoudspeakerLayout(juce::String name, std::vector<std::vector<float>> loudspeakers, float minX, float maxX, float minZ, float maxZ) :
        _name(name), _minX(minX), _minZ(minZ), _maxX(maxX), _maxZ(maxZ) {
        
        // store the array of N float array (x4) in an array of N "Loudspeakers" instance
        for (int i = 0; i < loudspeakers.size(); i++) {
            Loudspeakers l(i, loudspeakers[i][0], loudspeakers[i][1], loudspeakers[i][2], loudspeakers[i][3]);
            _loudspeakers.push_back(l);
        }
        
    
    }

    // utility method given the array of float array (x4) storing position and angle of each speaker.
    static  std::vector<std::vector<float>> calcSpkConfif48L() {

        std::vector<std::vector<float>> spkConfig;

        for (int channel = 0; channel < 48; channel++) {
            if (channel < 24) {
                std::vector<float> f{ 1.5f / 16.0f + channel * 1.5f / 8.0f, 0.0f,0.0f, 180.0f};
                spkConfig.push_back(f);
            }
            else {
                std::vector<float> f{ 4.5f, 0.0f,-1.5f / 16.0f - (channel - 24) * 1.5f / 8.0f, -90.0f};
                spkConfig.push_back(f);
            }

        }

        return spkConfig;
    }

    static  std::vector<std::vector<float>> calcSpkConfif16lin() {

        std::vector<std::vector<float>> spkConfig;
        float spkSize_JBL = 0.185f;
        float spkStartPositionX = -spkSize_JBL / 2.0f - 7 * spkSize_JBL;
;       for (int channel = 0; channel < 16; channel++) {        
            std::vector<float> f{ spkStartPositionX + channel * spkSize_JBL, 0.0f,0.0f, 180.0f };
            spkConfig.push_back(f);
        }

        return spkConfig;
    }
};


