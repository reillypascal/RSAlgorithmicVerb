// Plate reverb "in the style of Griesinger" from Dattorro 1997

#pragma once

#include <JuceHeader.h>

//#include "DelayLineWithSampleAccess.h"
#include "CustomDelays.h"
#include "LFO.h"
#include "ProcessorBase.h"
#include "Utilities.h"

class DattorroPlate : public ReverbProcessorBase
{
public:
    DattorroPlate();
    
    ~DattorroPlate() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    // parameter class
    ReverbProcessorParameters parameters;
    
    // allpasses
    juce::dsp::DelayLine<float> allpass1 {22050};
    juce::dsp::DelayLine<float> allpass2 {22050};
    juce::dsp::DelayLine<float> allpass3 {22050};
    juce::dsp::DelayLine<float> allpass4 {22050};
    DelayLineWithSampleAccess<float> allpass5 {22050};
    DelayLineWithSampleAccess<float> allpass6 {22050};
    // modulated allpasses
    juce::dsp::DelayLine<float> modulatedAPF1 {22050};
    juce::dsp::DelayLine<float> modulatedAPF2 {22050};
    // delays
//    juce::dsp::DelayLine<float> preDelay {22050};
    DelayLineWithSampleAccess<float> delay1 {22050};
    DelayLineWithSampleAccess<float> delay2 {22050};
    DelayLineWithSampleAccess<float> delay3 {22050};
    DelayLineWithSampleAccess<float> delay4 {22050};
    // lowpass filters
    juce::dsp::FirstOrderTPTFilter<float> inputFilter;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter1;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter2;
    
    OscillatorParameters lfoParameters;
    SignalGenData lfoOutput;
    LFO lfo;
    
    float allpassOutput = 0;
    float feedback = 0;
    float feedforward = 0;
    float summingA = 0;
    float summingB = 0;
    float channel0Output = 0;
    float channel1Output = 0;
    
//    float mPreDelayTime = 441;
//    float mSize = 1;
//    float mDecay = 0.25;
//    float mDampingCutoff = 6500;
//    float mDiffusion = 0.75;
//    float mEarlyLateMix = 1;
//    float mDryWetMix = 0.25;
};

//class DattorroPlate : public ProcessorBase
//{
//public:
//	DattorroPlate();
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
//	// allpasses
//	juce::dsp::DelayLine<float> allpass1 {22050};
//	juce::dsp::DelayLine<float> allpass2 {22050};
//	juce::dsp::DelayLine<float> allpass3 {22050};
//	juce::dsp::DelayLine<float> allpass4 {22050};
//	DelayLineWithSampleAccess<float> allpass5 {22050};
//	DelayLineWithSampleAccess<float> allpass6 {22050};
//	// modulated allpasses
//	juce::dsp::DelayLine<float> modulatedAPF1 {22050};
//	juce::dsp::DelayLine<float> modulatedAPF2 {22050};
//	// delays
//	juce::dsp::DelayLine<float> preDelay {22050};
//	DelayLineWithSampleAccess<float> delay1 {22050};
//	DelayLineWithSampleAccess<float> delay2 {22050};
//	DelayLineWithSampleAccess<float> delay3 {22050};
//	DelayLineWithSampleAccess<float> delay4 {22050};
//	// lowpass filters
//	juce::dsp::FirstOrderTPTFilter<float> inputFilter;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter1;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter2;
//	// mixers
//	juce::dsp::DryWetMixer<float> dryWetMixer;
//	
//	OscillatorParameters lfoParameters;
//	SignalGenData lfoOutput;
//	LFO lfo;
//	
//	float allpassOutput = 0;
//	float feedback = 0;
//	float feedforward = 0;
//	float summingA = 0;
//	float summingB = 0;
//	float channel0Output = 0;
//	float channel1Output = 0;
//	
//	float mPreDelayTime = 441;
//	float mSize = 1;
//	float mDecay = 0.25;
//	float mDampingCutoff = 6500;
//	float mDiffusion = 0.75;
//	float mEarlyLateMix = 1;
//	float mDryWetMix = 0.25;
//};
