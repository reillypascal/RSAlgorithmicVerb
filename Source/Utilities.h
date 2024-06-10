/*
  ==============================================================================

    Miscellaneous functions and classes

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

inline float scale(float input, float inLow, float inHi, float outLow, float outHi)
{
    float scaleFactor = (outHi - outLow)/(inHi - inLow);
    float offset = outLow - inLow;
    return (input * scaleFactor) + offset;
}

struct ReverbProcessorParameters
{
    ReverbProcessorParameters() {}
    
    ReverbProcessorParameters& operator=(const ReverbProcessorParameters& params)
    {
        if (this != &params)
        {
            roomSize = params.roomSize;
            decayTime = params.decayTime;
            damping = params.damping;
            diffusion = params.diffusion;
        }
        return *this;
    }
    
    bool operator==(const ReverbProcessorParameters& params)
    {
        if (params.roomSize == roomSize &&
            params.decayTime == decayTime &&
            params.damping == damping &&
            params.diffusion == diffusion)
            return true;
        
        return false;
    }
    
    // top row
    float roomSize { 1.0f };
    float decayTime { 0.35f };
    float damping { 20000.0f };
    float diffusion { 0.75f };
    // bottom row
    float preDelay { 0.0f };
    float lowCut { 20.0f };
    float highCut { 20000.0f };
    float earlyLateReflections { 1.0f };
    float dryWetMix { 0.35f };
};

