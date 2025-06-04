/*
  ==============================================================================
 
    Special-effect reverbs (delay/reverb hybrids, nonlinear decay times, etc.)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//#include "DelayLineWithSampleAccess.h"
#include "CustomDelays.h"
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
    ReverbProcessorParameters parameters;
    
    DelayLineWithSampleAccess<float> delay { 22050 };
    
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
    juce::dsp::FirstOrderTPTFilter<float> dcFilter;
    
    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    std::vector<LFO> lfo;
    
    std::vector<float> channelFeedback { };
    std::vector<std::vector<float>> channelOutDelayTimes { { 175, 60, 190, 137 }, { 160, 77, 212, 112 } };
    std::vector<float> feedbackDelayTimes { 47, 158, 200, 233, 120, 169, 209, 240, 134, 180, 217, 244, 146, 190, 225, 247 };
    
    float sampleRate { 44100 };
    float samplesPerMs { 44.1 };
};

//=====================================================================================

class EventHorizon : public ReverbProcessorBase
{
public:
    EventHorizon();
    
    ~EventHorizon() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    // parameter struct
    ReverbProcessorParameters parameters;
    
    std::vector<Allpass<float>> mainAllpasses { };
    std::vector<std::vector<Allpass<float>>> outAllpasses { };
    
    std::vector<juce::dsp::FirstOrderTPTFilter<float>> dampingFilters { };
    
    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    LFO lfo;
    
    std::vector<float> delayTimes = { 271, 2003, 337, 1487, 2719, 1109, 
                                      3121, 541, 3923, 1609, 701, 1303,
                                      2549, 439, 3583, 977, 1877, 211,
                                      2251, 487, 823, 2917, 3307, 3767 };
    
    std::vector<std::vector<float>> outDelayTimes = {{ 251, 919, 571, 389 },
                                                     { 241, 577, 911, 397 }};
    
    int numSeriesAllpasses { 24 };
    int numOutputAllpasses { 4 };
    
    std::vector<float> outputAllpassValues { };
    
    float outputScalar { 0.5f };
    float sampleRate { 44100 };
    float samplesPerMs { 44.1 };
};
