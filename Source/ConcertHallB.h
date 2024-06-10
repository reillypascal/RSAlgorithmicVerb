/*
  ==============================================================================

    ConcertHallB.h
    Created: 4 Jun 2023 4:38:39pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "DelayLineWithSampleAccess.h"
#include "LFO.h"
#include "ProcessorBase.h"
#include "Utilities.h"

class LargeConcertHallB : public ReverbProcessorBase
{
public:
    LargeConcertHallB();
    
    ~LargeConcertHallB() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:    
    // parameter class
    ReverbProcessorParameters mParameters;
    
    // filters
    juce::dsp::DelayLine<float> inputBandwidth { 4 };
    juce::dsp::DelayLine<float> feedbackDamping { 4 };
    juce::dsp::FirstOrderTPTFilter<float> loopDamping;
    // L
    juce::dsp::DelayLine<float> allpassChorusL { 1764 };
    // R
    juce::dsp::DelayLine<float> allpassChorusR { 1764 };

    // delays
    juce::dsp::DelayLine<float> inputZ { 4 };
    // L
    DelayLineWithSampleAccess<float> loopDelayL1 { 8 };
    DelayLineWithSampleAccess<float> loopDelayL2 { 4410 };
    DelayLineWithSampleAccess<float> loopDelayL3 { 4410 };
    DelayLineWithSampleAccess<float> loopDelayL4 { 4410 };
    // R
    DelayLineWithSampleAccess<float> loopDelayR1 { 8 };
    DelayLineWithSampleAccess<float> loopDelayR2 { 4410 };
    DelayLineWithSampleAccess<float> loopDelayR3 { 4410 };
    DelayLineWithSampleAccess<float> loopDelayR4 { 4410 };

    // allpasses
    // L
    juce::dsp::DelayLine<float> allpassL1 { 4410 };
    juce::dsp::DelayLine<float> allpassL2 { 4410 };
    juce::dsp::DelayLine<float> allpassL3Inner { 4410 };
    juce::dsp::DelayLine<float> allpassL3Outer { 4410 };
    juce::dsp::DelayLine<float> allpassL4Innermost { 4410 };
    juce::dsp::DelayLine<float> allpassL4Inner { 4410 };
    juce::dsp::DelayLine<float> allpassL4Outer { 4410 };
    // R
    juce::dsp::DelayLine<float> allpassR1 { 4410 };
    juce::dsp::DelayLine<float> allpassR2 { 4410 };
    juce::dsp::DelayLine<float> allpassR3Inner { 4410 };
    juce::dsp::DelayLine<float> allpassR3Outer { 4410 };
    juce::dsp::DelayLine<float> allpassR4Innermost { 4410 };
    juce::dsp::DelayLine<float> allpassR4Inner { 4410 };
    juce::dsp::DelayLine<float> allpassR4Outer { 4410 };

    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    LFO lfo;

    float allpassOutputInnermost = 0;
    float allpassOutputInner = 0;
    float allpassOutputOuter = 0;

    float feedforwardInnermost = 0;
    float feedforwardInner = 0;
    float feedforwardOuter = 0;

    float feedbackInnermost = 0;
    float feedbackInner = 0;
    float feedbackOuter = 0;

    std::vector<float> channelInput {0, 0};
    std::vector<float> channelFeedback {0, 0};
    std::vector<float> channelOutput {0, 0};
    
    int mSampleRate = 44100;
};

//class LargeConcertHallB : public ProcessorBase
//{
//public:
//	LargeConcertHallB();
//	
//	//==============================================================================
//	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
//	
//	//==============================================================================
//	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
//	
//	//==============================================================================
//	void reset() override;
//	
//	//==============================================================================
//	const juce::String getName() const override;
//	
//	//==============================================================================
//	void setSize(float newSize) override;
//	void setDecay(float newDecay) override;
//	void setDampingCutoff(float newCutoff) override;
//	void setDiffusion(float newDiffusion) override;
//	void setPreDelay(float newPreDelay) override;
//	void setEarlyLateMix(float newMix) override;
//	void setDryWetMix(float newMix) override;
//	
//private:
//	// filters
//	juce::dsp::DelayLine<float> inputBandwidth { 4 };
//	juce::dsp::DelayLine<float> feedbackDamping { 4 };
//	juce::dsp::FirstOrderTPTFilter<float> loopDamping;
//	// L
//	juce::dsp::DelayLine<float> allpassChorusL { 1764 };
//	// R
//	juce::dsp::DelayLine<float> allpassChorusR { 1764 };
//
//	// delays
//	juce::dsp::DelayLine<float> inputZ { 4 };
//	// L
//	DelayLineWithSampleAccess<float> loopDelayL1 { 8 };
//	DelayLineWithSampleAccess<float> loopDelayL2 { 4410 };
//	DelayLineWithSampleAccess<float> loopDelayL3 { 4410 };
//	DelayLineWithSampleAccess<float> loopDelayL4 { 4410 };
//	// R
//	DelayLineWithSampleAccess<float> loopDelayR1 { 8 };
//	DelayLineWithSampleAccess<float> loopDelayR2 { 4410 };
//	DelayLineWithSampleAccess<float> loopDelayR3 { 4410 };
//	DelayLineWithSampleAccess<float> loopDelayR4 { 4410 };
//	
//	// allpasses
//	// L
//	juce::dsp::DelayLine<float> allpassL1 { 4410 };
//	juce::dsp::DelayLine<float> allpassL2 { 4410 };
//	juce::dsp::DelayLine<float> allpassL3Inner { 4410 };
//	juce::dsp::DelayLine<float> allpassL3Outer { 4410 };
//	juce::dsp::DelayLine<float> allpassL4Innermost { 4410 };
//	juce::dsp::DelayLine<float> allpassL4Inner { 4410 };
//	juce::dsp::DelayLine<float> allpassL4Outer { 4410 };
//	// R
//	juce::dsp::DelayLine<float> allpassR1 { 4410 };
//	juce::dsp::DelayLine<float> allpassR2 { 4410 };
//	juce::dsp::DelayLine<float> allpassR3Inner { 4410 };
//	juce::dsp::DelayLine<float> allpassR3Outer { 4410 };
//	juce::dsp::DelayLine<float> allpassR4Innermost { 4410 };
//	juce::dsp::DelayLine<float> allpassR4Inner { 4410 };
//	juce::dsp::DelayLine<float> allpassR4Outer { 4410 };
//	
//	// mixers
//	juce::dsp::DryWetMixer<float> dryWetMixer;
//	
//	OscillatorParameters lfoParameters;
//	SignalGenData lfoOutput;
//	LFO lfo;
//	
//	float allpassOutputInnermost = 0;
//	float allpassOutputInner = 0;
//	float allpassOutputOuter = 0;
//	
//	float feedforwardInnermost = 0;
//	float feedforwardInner = 0;
//	float feedforwardOuter = 0;
//	
//	float feedbackInnermost = 0;
//	float feedbackInner = 0;
//	float feedbackOuter = 0;
//	
//	std::vector<float> channelInput {0, 0};
//	std::vector<float> channelFeedback {0, 0};
//	std::vector<float> channelOutput {0, 0};
//	
//	float mPreDelayTime = 441;
//	float mSize = 1;
//	float mDecay = 0.25;
//	float mDampingCutoff = 6500;
//	float mDiffusion = 0.75;
//	float mEarlyLateMix = 1;
//	float mDryWetMix = 0.25;
//};
