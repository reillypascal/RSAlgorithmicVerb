/*
  ==============================================================================

    

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "DelayLineWithSampleAccess.h"
#include "LFO.h"
#include "ProcessorBase.h"
#include "Utilities.h"

class Constellation : public ReverbProcessorBase
{
public:
    Constellation();
    
    ~Constellation() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    // parameter struct
    ReverbProcessorParameters mParameters;
    
    DelayLineWithSampleAccess<float> delay { 22050 };
    
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
    
    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    std::vector<LFO> lfo;
    
    std::vector<float> channelFeedback { };
    std::vector<std::vector<float>> channelOutDelayTimes { { 175, 60, 190, 137 }, { 160, 77, 212, 112 } };
    std::vector<float> feedbackDelayTimes { 47, 158, 200, 233, 120, 169, 209, 240, 134, 180, 217, 244, 146, 190, 225, 247 };
    
    float mSampleRate { 44100 };
    float mSamplesPerMs { 44.1 };
};
