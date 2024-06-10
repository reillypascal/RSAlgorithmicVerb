/*
  ==============================================================================

    "Freeverb"-style Schroeder reverb; 8 parallel combs -> 4 series allpasses, with added Dattorro FIR early reflections

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"
#include "Utilities.h"

class Freeverb : public ReverbProcessorBase
{
public:
    Freeverb();
    
    ~Freeverb() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    ReverbProcessorParameters mParameters;
    
    juce::dsp::DelayLine<float> comb0 {22050};
    juce::dsp::DelayLine<float> comb1 {22050};
    juce::dsp::DelayLine<float> comb2 {22050};
    juce::dsp::DelayLine<float> comb3 {22050};
    juce::dsp::DelayLine<float> comb4 {22050};
    juce::dsp::DelayLine<float> comb5 {22050};
    juce::dsp::DelayLine<float> comb6 {22050};
    juce::dsp::DelayLine<float> comb7 {22050};
    
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter0;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter1;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter2;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter3;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter4;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter5;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter6;
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter7;
    
    juce::dsp::DelayLine<float> allpass0 {1000};
    juce::dsp::DelayLine<float> allpass1 {1000};
    juce::dsp::DelayLine<float> allpass2 {1000};
    juce::dsp::DelayLine<float> allpass3 {1000};
    
    float comb0Output = 0;
    float comb1Output = 0;
    float comb2Output = 0;
    float comb3Output = 0;
    float comb4Output = 0;
    float comb5Output = 0;
    float comb6Output = 0;
    float comb7Output = 0;
    
    float allpass0Output = 0;
    float allpass1Output = 0;
    float allpass2Output = 0;
    float allpass3Output = 0;
    
    float feedback;
    float feedforward;
        
    float mWidth = 23;
};

//class Freeverb : public ProcessorBase
//{
//public:
//	Freeverb();
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
//	const juce::String getName() const override { return "Freeverb"; }
//	
//	//==============================================================================
//	void setSize(float newSize) override;
//	void setDecay(float newDecay) override;
//	void setDampingCutoff(float newCutoff) override;
//	void setPreDelay(float newPreDelay) override;
//	void setEarlyLateMix(float newMix) override;
//	void setDryWetMix(float newMix) override;
//	
//private:
////	EarlyReflections earlyReflections;
//	
//	juce::dsp::DelayLine<float> comb0 {22050};
//	juce::dsp::DelayLine<float> comb1 {22050};
//	juce::dsp::DelayLine<float> comb2 {22050};
//	juce::dsp::DelayLine<float> comb3 {22050};
//	juce::dsp::DelayLine<float> comb4 {22050};
//	juce::dsp::DelayLine<float> comb5 {22050};
//	juce::dsp::DelayLine<float> comb6 {22050};
//	juce::dsp::DelayLine<float> comb7 {22050};
//	
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter0;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter1;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter2;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter3;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter4;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter5;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter6;
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter7;
//	
//	juce::dsp::DelayLine<float> allpass0 {1000};
//	juce::dsp::DelayLine<float> allpass1 {1000};
//	juce::dsp::DelayLine<float> allpass2 {1000};
//	juce::dsp::DelayLine<float> allpass3 {1000};
//	
//	float comb0Output = 0;
//	float comb1Output = 0;
//	float comb2Output = 0;
//	float comb3Output = 0;
//	float comb4Output = 0;
//	float comb5Output = 0;
//	float comb6Output = 0;
//	float comb7Output = 0;
//	
//	float allpass0Output = 0;
//	float allpass1Output = 0;
//	float allpass2Output = 0;
//	float allpass3Output = 0;
//	
//	float feedback;
//	float feedforward;
//	
//	juce::dsp::DryWetMixer<float> mixer;
//	
//	float mPreDelayTime = 441;
//	float mSize = 1;
//	float mDecay = 0.25;
//	float mDampingCutoff = 6500;
//	float mEarlyLateMix = 1;
//	float mDryWetMix = 0.25;
//	float mWidth = 23;
//};
