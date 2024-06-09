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
            preDelay = params.preDelay;
            lowCut = params.lowCut;
            highCut = params.highCut;
            earlyLateReflections = params.earlyLateReflections;
            dryWetMix = params.dryWetMix;
        }
        return *this;
    }
    
    bool operator==(const ReverbProcessorParameters& params)
    {
        if (params.damping == damping &&
            params.decayTime == decayTime &&
            params.diffusion == diffusion &&
            params.dryWetMix == dryWetMix &&
            params.earlyLateReflections == earlyLateReflections &&
            params.highCut == highCut &&
            params.lowCut == lowCut &&
            params.preDelay == preDelay &&
            params.roomSize == roomSize)
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

