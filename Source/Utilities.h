/*
  ==============================================================================

    Miscellaneous functions and classes

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ReverbProcessorParameters
{
    ReverbProcessorParameters() {}
    
    ReverbProcessorParameters& operator=(const ReverbProcessorParameters& params)
    {
        if (this != &params)
        {
            
        }
        return *this;
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

