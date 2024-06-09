/*
  ==============================================================================

	Allpass loop rooms from Gardner 1992

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"
#include "Utilities.h"


//==============================================================================
class GardnerSmallRoom :public ReverbProcessorBase
{
public:
    GardnerSmallRoom();
    
    ~GardnerSmallRoom() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    // parameter struct
    ReverbProcessorParameters mParameters;
    
    juce::dsp::DelayLine<float> delay1 { 22050 };
    juce::dsp::DelayLine<float> delay2 { 22050 };
    juce::dsp::DelayLine<float> delay3 { 22050 };
    juce::dsp::DelayLine<float> delay4 { 22050 };
    juce::dsp::DelayLine<float> delay5 { 22050 };
    juce::dsp::DelayLine<float> delay6 { 22050 };
    
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
    
    juce::dsp::DryWetMixer<float> dryWetMixer;
    
    float allpassOutputInner = 0;
    float allpassOutputOuter = 0;
    float feedforwardInner = 0;
    float feedforwardOuter = 0;
    float feedbackInner = 0;
    float feedbackOuter = 0;
    std::vector<float> channelFeedback {0, 0};
    std::vector<float> channelOutput {0, 0};
    
    std::vector<float> channelDelayOffset {0, 7};
    
    int mSampleRate { 44100 };
};


//==============================================================================
class GardnerMediumRoom : public ReverbProcessorBase
{
public:
    GardnerMediumRoom();
    
    ~GardnerMediumRoom() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    // parameter struct
    ReverbProcessorParameters mParameters;
    
    juce::dsp::DelayLine<float> delay1 { 22050 };
    juce::dsp::DelayLine<float> delay2 { 22050 };
    juce::dsp::DelayLine<float> delay3 { 22050 };
    juce::dsp::DelayLine<float> delay4 { 22050 };
    juce::dsp::DelayLine<float> delay5 { 22050 };
    juce::dsp::DelayLine<float> delay6 { 22050 };
    juce::dsp::DelayLine<float> delay7 { 22050 };
    juce::dsp::DelayLine<float> delay8 { 22050 };
    juce::dsp::DelayLine<float> delay9 { 22050 };
    juce::dsp::DelayLine<float> delay10 { 22050 };
    
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
    
    juce::dsp::DryWetMixer<float> dryWetMixer;
    
    float allpassOutputInner = 0;
    float allpassOutputOuter = 0;
    float feedforwardInner = 0;
    float feedforwardOuter = 0;
    float feedbackInner = 0;
    float feedbackOuter = 0;
    std::vector<float> channelFeedback {0, 0};
    std::vector<float> channelOutput {0, 0};
    
    std::vector<float> channelDelayOffset {0, 15};
    
    int mSampleRate { 44100 };
};


//==============================================================================
class GardnerLargeRoom : public ReverbProcessorBase
{
public:
    GardnerLargeRoom();
    
    ~GardnerLargeRoom() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
private:
    // parameter struct
    ReverbProcessorParameters mParameters;
    
    juce::dsp::DelayLine<float> delay1 { 22050 };
    juce::dsp::DelayLine<float> delay2 { 22050 };
    juce::dsp::DelayLine<float> delay3 { 22050 };
    juce::dsp::DelayLine<float> delay4 { 22050 };
    juce::dsp::DelayLine<float> delay5 { 22050 };
    juce::dsp::DelayLine<float> delay6 { 22050 };
    juce::dsp::DelayLine<float> delay7 { 22050 };
    juce::dsp::DelayLine<float> delay8 { 22050 };
    juce::dsp::DelayLine<float> delay9 { 22050 };
    juce::dsp::DelayLine<float> delay10 { 22050 };
    juce::dsp::DelayLine<float> delay11 { 22050 };
    
    juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
    
    juce::dsp::DryWetMixer<float> dryWetMixer;
    
    float allpassOutputInner = 0;
    float allpassOutputOuter = 0;
    float feedforwardInner = 0;
    float feedforwardOuter = 0;
    float feedbackInner = 0;
    float feedbackOuter = 0;
    std::vector<float> channelFeedback {0, 0};
    std::vector<float> channelOutput {0, 0};
    
    std::vector<float> channelDelayOffset {0, 23};
    
    int mSampleRate { 44100 };
};

//class GardnerSmallRoom : public ProcessorBase
//{
//public:
//	GardnerSmallRoom();
//	
//	//==============================================================================
//	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
//	
//	//==============================================================================
//	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
//	
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
//	juce::dsp::DelayLine<float> delay1 { 22050 };
//	juce::dsp::DelayLine<float> delay2 { 22050 };
//	juce::dsp::DelayLine<float> delay3 { 22050 };
//	juce::dsp::DelayLine<float> delay4 { 22050 };
//	juce::dsp::DelayLine<float> delay5 { 22050 };
//	juce::dsp::DelayLine<float> delay6 { 22050 };
//	
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
//	
//	juce::dsp::DryWetMixer<float> dryWetMixer;
//	
//	float allpassOutputInner = 0;
//	float allpassOutputOuter = 0;
//	float feedforwardInner = 0;
//	float feedforwardOuter = 0;
//	float feedbackInner = 0;
//	float feedbackOuter = 0;
//	std::vector<float> channelFeedback {0, 0};
//	std::vector<float> channelOutput {0, 0};
//	
//	std::vector<float> channelDelayOffset {0, 7};
//	
//	float mPreDelayTime = 441;
//	float mSize = 1;
//	float mDecay = 0.25;
//	float mDampingCutoff = 6500;
//	float mDiffusion = 0.75;
//	float mEarlyLateMix = 1;
//	float mDryWetMix = 0.25;
//};

////==============================================================================
//class GardnerMediumRoom : public ProcessorBase
//{
//public:
//	GardnerMediumRoom();
//	
//	//==============================================================================
//	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
//	
//	//==============================================================================
//	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
//	
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
//	juce::dsp::DelayLine<float> delay1 { 22050 };
//	juce::dsp::DelayLine<float> delay2 { 22050 };
//	juce::dsp::DelayLine<float> delay3 { 22050 };
//	juce::dsp::DelayLine<float> delay4 { 22050 };
//	juce::dsp::DelayLine<float> delay5 { 22050 };
//	juce::dsp::DelayLine<float> delay6 { 22050 };
//	juce::dsp::DelayLine<float> delay7 { 22050 };
//	juce::dsp::DelayLine<float> delay8 { 22050 };
//	juce::dsp::DelayLine<float> delay9 { 22050 };
//	juce::dsp::DelayLine<float> delay10 { 22050 };
//	
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
//	
//	juce::dsp::DryWetMixer<float> dryWetMixer;
//	
//	float allpassOutputInner = 0;
//	float allpassOutputOuter = 0;
//	float feedforwardInner = 0;
//	float feedforwardOuter = 0;
//	float feedbackInner = 0;
//	float feedbackOuter = 0;
//	std::vector<float> channelFeedback {0, 0};
//	std::vector<float> channelOutput {0, 0};
//	
//	std::vector<float> channelDelayOffset {0, 15};
//	
//	float mPreDelayTime = 441;
//	float mSize = 1;
//	float mDecay = 0.25;
//	float mDampingCutoff = 6500;
//	float mDiffusion = 0.75;
//	float mEarlyLateMix = 1;
//	float mDryWetMix = 0.25;
//};

////==============================================================================
//class GardnerLargeRoom : public ProcessorBase
//{
//public:
//	GardnerLargeRoom();
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
//	const juce::String getName() const override { return "GardnerLargeRoom"; }
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
//	juce::dsp::DelayLine<float> delay1 { 22050 };
//	juce::dsp::DelayLine<float> delay2 { 22050 };
//	juce::dsp::DelayLine<float> delay3 { 22050 };
//	juce::dsp::DelayLine<float> delay4 { 22050 };
//	juce::dsp::DelayLine<float> delay5 { 22050 };
//	juce::dsp::DelayLine<float> delay6 { 22050 };
//	juce::dsp::DelayLine<float> delay7 { 22050 };
//	juce::dsp::DelayLine<float> delay8 { 22050 };
//	juce::dsp::DelayLine<float> delay9 { 22050 };
//	juce::dsp::DelayLine<float> delay10 { 22050 };
//	juce::dsp::DelayLine<float> delay11 { 22050 };
//	
//	juce::dsp::FirstOrderTPTFilter<float> dampingFilter;
//	
//	juce::dsp::DryWetMixer<float> dryWetMixer;
//	
//	float allpassOutputInner = 0;
//	float allpassOutputOuter = 0;
//	float feedforwardInner = 0;
//	float feedforwardOuter = 0;
//	float feedbackInner = 0;
//	float feedbackOuter = 0;
//	std::vector<float> channelFeedback {0, 0};
//	std::vector<float> channelOutput {0, 0};
//	
//	std::vector<float> channelDelayOffset {0, 23};
//	
//	float mPreDelayTime = 441;
//	float mSize = 1;
//	float mDecay = 0.25;
//	float mDampingCutoff = 6500;
//	float mDiffusion = 0.75;
//	float mEarlyLateMix = 1;
//	float mDryWetMix = 0.25;
//};
