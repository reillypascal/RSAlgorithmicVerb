/*
  ==============================================================================

    Collection of FDN (feedback delay network) algorithms

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "LFO.h"
#include "ProcessorBase.h"
#include "Utilities.h"

class GeneralizedFDN : public ReverbProcessorBase
{
public:
    GeneralizedFDN();
    
    GeneralizedFDN(int order, std::string type);
    
    ~GeneralizedFDN() override;
    
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
    
    // hold output/feedback signals
    std::vector<std::vector<float>> delayOutputMatrix { };
    std::vector<std::vector<float>> feedbackSigMatrix { };
    
    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    std::vector<LFO> lfo;
    
    float feedbackScalar { sqrt(2.0f) };
    
    std::vector<std::vector<float>> feedbackMatrix {
        { 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0, 0, 0, 0, 0, 0, 1, -1 },
        { 1, 1, 0, 0, 0, 0, 0, 0 },
        { 1, -1, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, -1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 1, -1, 0, 0 }};
    
    std::vector<int> delayTimes { 561, 2664, 410, 1343, 210, 3931, 158, 995 };
    
    std::vector<int> inDelays { 4, 6 };
    std::vector<int> modDelays { 1, 3 };
    
    int delayCount { 8 };
};

//===================================================================

class SP82FDN : public ReverbProcessorBase
{
public:
    SP82FDN();
    
    ~SP82FDN() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    
    
    const std::vector<std::vector<float>> feedbackSPMatrix {
        { 0, 1, 1, 0 },
        { -1, 0, 0, -1 },
        { 1, 0, 0, -1 },
        { 0, 1, -1, 0 }};
};

//class Anderson8xFDN : public ReverbProcessorBase
//{
//public:
//    Anderson8xFDN();
//    
//    ~Anderson8xFDN() override;
//    
//    void prepare(const juce::dsp::ProcessSpec& spec) override;
//    
//    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
//    
//    void reset() override;
//    
//    ReverbProcessorParameters& getParameters() override;
//    
//    void setParameters(const ReverbProcessorParameters& params) override;
//private:
//    // parameter class
//    ReverbProcessorParameters mParameters;
//    
//    // delays
//    std::vector<juce::dsp::DelayLine<float>> delays {};
//    
//    // lowpass filters
//    std::vector<juce::dsp::FirstOrderTPTFilter<float>> dampingFilters {};
//    
//    // hold output/feedback signals
//    std::vector<std::vector<float>> delayOutputMatrix { };
//    std::vector<std::vector<float>> feedbackSigMatrix { };
//    
//    OscillatorParameters lfoParameters;
//    SignalGenData lfoOutput;
//    std::vector<LFO> lfo;
//    
//    const std::vector<std::vector<float>> feedbackAndersonMatrix {
//        { 0, 0, 0, 0, 0, 0, 1, 1 },
//        { 0, 0, 0, 0, 0, 0, 1, -1 },
//        { 1, 1, 0, 0, 0, 0, 0, 0 },
//        { 1, -1, 0, 0, 0, 0, 0, 0 },
//        { 0, 0, 1, 1, 0, 0, 0, 0 },
//        { 0, 0, 1, -1, 0, 0, 0, 0 },
//        { 0, 0, 0, 0, 1, 1, 0, 0 },
//        { 0, 0, 0, 0, 1, -1, 0, 0 }};
//    
//    const std::vector<int> delayTimes { 561, 2664, 410, 1343, 210, 3931, 158, 995 };
//    
//    const std::vector<int> inDelays { 4, 6 };
//    const std::vector<int> modDelays { 1, 3 };
//    
//    const int delayCount { 8 };
//    const float sqrt2 = sqrt(2.0);
//};
//
////===================================================================
//
//class Hadamard8xFDN : public ReverbProcessorBase
//{
//public:
//    Hadamard8xFDN();
//    
//    ~Hadamard8xFDN() override;
//    
//    void prepare(const juce::dsp::ProcessSpec& spec) override;
//    
//    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
//    
//    void reset() override;
//    
//    ReverbProcessorParameters& getParameters() override;
//    
//    void setParameters(const ReverbProcessorParameters& params) override;
//private:
//    // parameter class
//    ReverbProcessorParameters mParameters;
//    
//    // delays
//    std::vector<juce::dsp::DelayLine<float>> delays {};
//    
//    // lowpass filters
//    std::vector<juce::dsp::FirstOrderTPTFilter<float>> dampingFilters {};
//    
//    // hold output/feedback signals
//    std::vector<std::vector<float>> delayOutputMatrix { };
//    std::vector<std::vector<float>> feedbackSigMatrix { };
//    
//    OscillatorParameters lfoParameters;
//    SignalGenData lfoOutput;
//    std::vector<LFO> lfo;
//        
//    const std::vector<std::vector<float>> feedbackHadamardMatrix {
//        { 0.3536,   0.3536,   0.3536,   0.3536,   0.3536,   0.3536,   0.3536,   0.3536 },
//        { 0.3536,  -0.3536,   0.3536,  -0.3536,   0.3536,  -0.3536,   0.3536,  -0.3536 },
//        { 0.3536,   0.3536,  -0.3536,  -0.3536,   0.3536,   0.3536,  -0.3536,  -0.3536 },
//        { 0.3536,  -0.3536,  -0.3536,   0.3536,   0.3536,  -0.3536,  -0.3536,   0.3536 },
//        { 0.3536,   0.3536,   0.3536,   0.3536,  -0.3536,  -0.3536,  -0.3536,  -0.3536 },
//        { 0.3536,  -0.3536,   0.3536,  -0.3536,  -0.3536,   0.3536,  -0.3536,   0.3536 },
//        { 0.3536,   0.3536,  -0.3536,  -0.3536,  -0.3536,  -0.3536,   0.3536,   0.3536 },
//        { 0.3536,  -0.3536,  -0.3536,   0.3536,  -0.3536,   0.3536,   0.3536,  -0.3536 }};
//    
//    const std::vector<int> delayTimes { 271, 487, 823, 1487, 2003, 2719, 3203, 3923 };
//    
//    const std::vector<int> inDelays { 0, 1 };
//    const std::vector<int> modDelays { 3, 7 };
//    
//    const int delayCount { 8 };
//};
//
////===================================================================
//class Householder8xFDN : public ReverbProcessorBase
//{
//public:
//    Householder8xFDN();
//    
//    ~Householder8xFDN() override;
//    
//    void prepare(const juce::dsp::ProcessSpec& spec) override;
//    
//    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
//    
//    void reset() override;
//    
//    ReverbProcessorParameters& getParameters() override;
//    
//    void setParameters(const ReverbProcessorParameters& params) override;
//private:
//    // parameter class
//    ReverbProcessorParameters mParameters;
//    
//    // delays
//    std::vector<juce::dsp::DelayLine<float>> delays {};
//    
//    // lowpass filters
//    std::vector<juce::dsp::FirstOrderTPTFilter<float>> dampingFilters {};
//    
//    // hold output/feedback signals
//    std::vector<std::vector<float>> delayOutputMatrix { };
//    std::vector<std::vector<float>> feedbackSigMatrix { };
//    
//    OscillatorParameters lfoParameters;
//    SignalGenData lfoOutput;
//    std::vector<LFO> lfo;
//        
//    const std::vector<std::vector<float>> feedbackHouseholderMatrix {
//        {  0.900750,  -0.190233,  -0.220427,  -0.240878,  -0.141037,  -0.141540,  -0.049417,  -0.059027 },
//        { -0.190233,   0.635382,  -0.422492,  -0.461690,  -0.270325,  -0.271289,  -0.094718,  -0.113138 },
//        { -0.220427,  -0.422492,   0.510449,  -0.534971,  -0.313231,  -0.314348,  -0.109751,  -0.131095 },
//        { -0.240878,  -0.461690,  -0.534971,   0.415394,  -0.342293,  -0.343514,  -0.119934,  -0.143258 },
//        { -0.141037,  -0.270325,  -0.313231,  -0.342293,   0.799584,  -0.201131,  -0.070223,  -0.083879 },
//        { -0.141540,  -0.271289,  -0.314348,  -0.343514,  -0.201131,   0.798152,  -0.070473,  -0.084178 },
//        { -0.049417,  -0.094718,  -0.109751,  -0.119934,  -0.070223,  -0.070473,   0.975395,  -0.029390 },
//        { -0.059027,  -0.113138,  -0.131095,  -0.143258,  -0.083879,  -0.084178,  -0.029390,   0.964894 }};
//    
//    const std::vector<int> delayTimes { 271, 487, 823, 1487, 2003, 2719, 3203, 3923 };
//    
//    const std::vector<int> inDelays { 0, 1 };
//    const std::vector<int> modDelays { 3, 7 };
//    
//    const int delayCount { 8 };
//};
//
