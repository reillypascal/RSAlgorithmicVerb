// Collection of FDN (feedback delay network) algorithms

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
    ReverbProcessorParameters parameters;

    // delays
    std::vector<juce::dsp::DelayLine<float>> delays{};

    // lowpass filters
    std::vector<juce::dsp::FirstOrderTPTFilter<float>> dampingFilters{};

    // hold output/feedback signals
    std::vector<std::vector<float>> delayOutputMatrix{};
    std::vector<std::vector<float>> feedbackSigMatrix{};

    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    std::vector<LFO> lfo;

    float feedbackScalar = sqrt(2.0f);

    std::vector<std::vector<float>> feedbackMatrix{
        {0, 0, 0, 0, 0, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 1, -1}, {1, 1, 0, 0, 0, 0, 0, 0}, {1, -1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, -1, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 1, -1, 0, 0}};

    std::vector<int> delayTimes{561, 2664, 410, 1343, 210, 3931, 158, 995};

    std::vector<int> inDelays{4, 6};
    std::vector<int> modDelays{1, 3};

    int delayCount = 8;
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
    const std::vector<std::vector<float>> feedbackSPMatrix{{0, 1, 1, 0}, {-1, 0, 0, -1}, {1, 0, 0, -1}, {0, 1, -1, 0}};
};
