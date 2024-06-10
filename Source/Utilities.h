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
            damping = params.damping;
            decayTime = params.decayTime;
            diffusion = params.diffusion;
            modDepth = params.modDepth;
            modRate = params.modRate;
            roomSize = params.roomSize;
        }
        return *this;
    }
    
    bool operator==(const ReverbProcessorParameters& params)
    {
        if (params.damping == damping &&
            params.decayTime == decayTime &&
            params.diffusion == diffusion &&
            params.modDepth == modDepth &&
            params.modRate == modRate &&
            params.roomSize == roomSize)
            return true;
        
        return false;
    }
    
    // top row
    float damping { 20000.0f };
    float decayTime { 0.35f };
    float diffusion { 0.75f };
    float modDepth { 0 };
    float modRate { 0.35f };
    float roomSize { 1.0f };
};

