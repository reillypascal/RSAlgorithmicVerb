/*
  ==============================================================================

    FDN based on "Designing Multi-Channel Reverberators" by "John Stautner and Miller Puckette

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "LFO.h"
#include "ProcessorBase.h"
#include "Utilities.h"

class Anderson8xFDN : public ReverbProcessorBase
{
public:
    Anderson8xFDN();
    
    ~Anderson8xFDN() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    // parameter class
    ReverbProcessorParameters mParameters;
    
    // delays
    std::vector<juce::dsp::DelayLine<float>> delays {};
    
    // lowpass filters
    std::vector<juce::dsp::FirstOrderTPTFilter<float>> dampingFilters {};
    
    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    std::vector<LFO> lfo;
    
//    std::vector<std::vector<float>> feedbackGainMatrix {
//        { 0, 1, 1, 0 },
//        { -1, 0, 0, -1 },
//        { 1, 0, 0, -1 },
//        { 0, 1, -1, 0 }};
    
//    std::vector<std::vector<float>> feedbackAndersonMatrix {
//        { 0, 0, 1, 1 },
//        { 0, 0, 1, -1 },
//        { 1, 1, 0, 0 },
//        { 1, -1, 0, 0 }};
    
    std::vector<std::vector<float>> feedbackAndersonMatrix {
        { 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0, 0, 0, 0, 0, 0, 1, -1 },
        { 1, 1, 0, 0, 0, 0, 0, 0 },
        { 1, -1, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, -1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 1, -1, 0, 0 }};
    
    std::vector<std::vector<float>> delayOutputMatrix { };
    std::vector<std::vector<float>> feedbackSigMatrix { };
    
    int delayCount { 8 };
//    std::vector<int> delayTimes { 1116, 1277, 1491, 1617 }; // every other from freeverb
    std::vector<int> delayTimes { 561, 2664, 410, 1343, 210, 3931, 158, 995 };
//    std::vector<int> delayTimes { 561, 2664, 1343, 995 };
};
