/*
  ==============================================================================
 
	FIR-based early reflections with 6 taps and HRTF for binaural stereo. Based on Dattorro
 
 TODO:
 - implement predelay
 - diffusion?

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//#include "DelayLineWithSampleAccess.h"
#include "CustomDelays.h"
#include "ProcessorBase.h"
#include "Utilities.h"

class EarlyReflections : public ReverbProcessorBase
{
public:
    EarlyReflections();
    
    ~EarlyReflections() override;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void reset() override;
    
    ReverbProcessorParameters& getParameters() override;
    
    void setParameters(const ReverbProcessorParameters& params) override;
    
    void setMonoFlag(const bool newMonoFlag) { monoFlag = newMonoFlag; }
private:
    ReverbProcessorParameters parameters;
    
    DelayLineWithSampleAccess<float> earlyReflectionsDelayLine {22050};
    juce::dsp::DelayLine<float> leftHRTFDelay {441};
    juce::dsp::DelayLine<float> rightHRTFDelay {441};
    juce::dsp::FirstOrderTPTFilter<float> leftHRTFFilter;
    juce::dsp::FirstOrderTPTFilter<float> rightHRTFFilter;
    
    float channel0Output { 0 };
    float channel1Output { 0 };
    
    float mPreDelayTime { 441 };
    float mInitialLevel { 1.0 };
    
    bool monoFlag { false };
};

////==============================================================================
//class EarlyReflections : public ProcessorBase
//{
//public:
//	EarlyReflections() {}
//	
//	void prepareToPlay(double sampleRate, int samplesPerBlock) override
//	{
//		juce::dsp::ProcessSpec monoSpec;
//		monoSpec.sampleRate = sampleRate;
//		monoSpec.maximumBlockSize = samplesPerBlock;
//		monoSpec.numChannels = 1;
//		
//		earlyReflectionsDelayLine.prepare(monoSpec);
//		
//		leftHRTFDelay.prepare(monoSpec);
//		rightHRTFDelay.prepare(monoSpec);
//		
//		leftHRTFFilter.prepare(monoSpec);
//		rightHRTFFilter.prepare(monoSpec);
//	}
//	
//	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
//	{
//		juce::ScopedNoDenormals noDenormals;
//		auto totalNumInputChannels = getTotalNumInputChannels();
//		auto totalNumOutputChannels = getTotalNumOutputChannels();
//		
//		for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//			buffer.clear (i, 0, buffer.getNumSamples());
//		
//		juce::AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
//		monoBuffer.clear();
//		
//		monoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
//		if(totalNumInputChannels > 1)
//		{
//			monoBuffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
//			monoBuffer.applyGain(0.5f);
//		}
//		
//		leftHRTFDelay.setDelay(35);
//		rightHRTFDelay.setDelay(35);
//		
//		auto* channelData = monoBuffer.getWritePointer(0);
//		for (int sample = 0; sample < monoBuffer.getNumSamples(); ++sample)
//		{
//			earlyReflectionsDelayLine.pushSample(0, channelData[sample]);
//			
//			// sum left 3 taps
//			channel0Output = earlyReflectionsDelayLine.getSampleAtDelay(0, 441 * mSize) * mInitialLevel;
//			channel0Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 2929 * mSize) * mInitialLevel * mDecay;
//			channel0Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 6319 * mSize) * mInitialLevel * pow(mDecay, 2);
//			
//			// sum right 3 taps (interleaved w/ left)
//			channel1Output = earlyReflectionsDelayLine.getSampleAtDelay(0, 1191 * mSize) * mInitialLevel;
//			channel1Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 3948 * mSize) * mInitialLevel * mDecay;
//			channel1Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 9462 * mSize) * mInitialLevel * pow(mDecay, 2);
//			
//			// right into left HRTF and vice versa
//			leftHRTFDelay.pushSample(0, channel1Output);
//			rightHRTFDelay.pushSample(0, channel0Output);
//			
//			// filter HRTFs and add to outputs
//			channel0Output += leftHRTFFilter.processSample(0, leftHRTFDelay.popSample(0));
//			channel1Output += rightHRTFFilter.processSample(0, rightHRTFDelay.popSample(0));
//			
//			// outputs into original stereo buffer
//			for (int destChannel = 0; destChannel < totalNumInputChannels; ++destChannel)
//			{
//				if (destChannel == 0)
//					buffer.setSample(0, sample, channel0Output);
//				else if (destChannel == 1)
//					buffer.setSample(1, sample, channel1Output);
//			}
//		}
//	}
//	
//	void reset() override
//	{
//		earlyReflectionsDelayLine.reset();
//		leftHRTFDelay.reset();
//		rightHRTFDelay.reset();
//		leftHRTFFilter.reset();
//		rightHRTFFilter.reset();
//	}
//	
//	//==============================================================================
//	const juce::String getName() const override { return "EarlyReflections"; }
//	
//	//==============================================================================
//	void setSize(float newSize) override { mSize = newSize; }
//	void setDecay(float newDecay) override { mDecay = newDecay; }
//	void setPreDelay(float newPreDelay) override { mPreDelayTime = newPreDelay; }
//	
//private:
//	DelayLineWithSampleAccess<float> earlyReflectionsDelayLine {22050};
//	juce::dsp::DelayLine<float> leftHRTFDelay {441};
//	juce::dsp::DelayLine<float> rightHRTFDelay {441};
//	juce::dsp::FirstOrderTPTFilter<float> leftHRTFFilter;
//	juce::dsp::FirstOrderTPTFilter<float> rightHRTFFilter;
//	
//	float channel0Output = 0;
//	float channel1Output = 0;
//	
//	float mPreDelayTime = 441;
//	float mSize = 1;
//	float mDecay = 0.3;
//	float mInitialLevel = 0.5;
//};
