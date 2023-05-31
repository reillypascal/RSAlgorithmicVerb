/*
  ==============================================================================

    GardnerRooms.h
    Created: 31 May 2023 12:03:37am
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"
#include "EarlyReflections.h"

class GardnerSmallRoom : public ProcessorBase
{
public:
	GardnerSmallRoom() {}
	
	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		// prepare stereo processors
		juce::dsp::ProcessSpec spec;
		spec.sampleRate = sampleRate;
		spec.maximumBlockSize = samplesPerBlock;
		spec.numChannels = getMainBusNumInputChannels();
		
		dryWetMixer.prepare(spec);
		
		delay1.prepare(spec);
		delay2.prepare(spec);
		delay3.prepare(spec);
		delay4.prepare(spec);
		delay5.prepare(spec);
		delay6.prepare(spec);
		
		dampingFilter.prepare(spec);
		dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
	}
	
	//==============================================================================
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
	{
		juce::ScopedNoDenormals noDenormals;
		
		juce::dsp::AudioBlock<float> dryBlock { buffer };
		dryWetMixer.setWetMixProportion(mDryWetMix);
		dryWetMixer.pushDrySamples(dryBlock);
		
		float samplesPerMs = getSampleRate() / 1000;
		
		delay1.setDelay(24 * samplesPerMs * mSize);
		delay2.setDelay(22 * samplesPerMs * mSize);
		delay3.setDelay(8.3 * samplesPerMs * mSize);
		delay4.setDelay(4.7 * samplesPerMs * mSize);
		delay5.setDelay(30 * samplesPerMs * mSize);
		delay6.setDelay(36 * samplesPerMs * mSize);
		
		dampingFilter.setCutoffFrequency(mDampingCutoff);
		
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		{
			auto* channelData = buffer.getWritePointer(channel);
			
			for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
			{
				channelData[sample] += dampingFilter.processSample(channel, channelFeedback.at(channel)) * mDecay;
				
				delay1.pushSample(channel, channelData[sample]);
				channelData[sample] = delay1.popSample(channel);
				
				// outer 35ms allpass
				feedforwardOuter = channelData[sample];
				allpassOutputOuter = delay4.popSample(channel);
				feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
				
				// inner 22ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay2.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.4)) * 0.4;
				delay2.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.4);
				
				// inner 8.3ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay3.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.6)) * 0.6;
				delay3.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.6);
				
				// finish outer 35ms allpass
				delay4.pushSample(channel, channelData[sample] + feedbackOuter);
				channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
				
				// mix into output
				channelOutput.at(channel) = channelData[sample] * 0.707;
				
				// outer 66ms allpass
				feedforwardOuter = channelData[sample];
				allpassOutputOuter = delay6.popSample(channel);
				feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.1)) * 0.1;
				
				// inner 30ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay5.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.4)) * 0.4;
				delay5.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.4);
				
				// finish outer 66ms allpass
				delay6.pushSample(channel, channelData[sample] + feedbackOuter);
				channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.1);
				
				// make channel feedback; mix output
				channelFeedback.at(channel) = channelData[sample];
				channelOutput.at(channel) += channelData[sample] * 0.707;
				channelData[sample] = channelOutput.at(channel);
			}
		}
		
		juce::dsp::AudioBlock<float> wetBlock { buffer };
		dryWetMixer.mixWetSamples(wetBlock);
	}
	
	void reset() override
	{
		dryWetMixer.reset();
		
		delay1.reset();
		delay2.reset();
		delay3.reset();
		delay4.reset();
		delay5.reset();
		delay6.reset();
		
		dampingFilter.reset();
	}
	
	//==============================================================================
	const juce::String getName() const override { return "GardnerSmallRoom"; }
	
	//==============================================================================
	void setSize(float newSize) override { mSize = newSize; }
	void setDecay(float newDecay) override { mDecay = newDecay; }
	void setDampingCutoff(float newCutoff) override { mDampingCutoff = newCutoff; }
	void setDiffusion(float newDiffusion) override { mDiffusion = newDiffusion; }
	void setPreDelay(float newPreDelay) override { mPreDelayTime = newPreDelay; }
	void setEarlyLateMix(float newMix) override { mEarlyLateMix = newMix; }
	void setDryWetMix(float newMix) override { mDryWetMix = newMix; }
	
private:
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
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
};

//==============================================================================
class GardnerMediumPlate : public ProcessorBase
{
public:
	GardnerMediumPlate() {}
	
	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		// prepare stereo processors
		juce::dsp::ProcessSpec spec;
		spec.sampleRate = sampleRate;
		spec.maximumBlockSize = samplesPerBlock;
		spec.numChannels = getMainBusNumInputChannels();
		
		dryWetMixer.prepare(spec);
		
		delay1.prepare(spec);
		delay2.prepare(spec);
		delay3.prepare(spec);
		delay4.prepare(spec);
		delay5.prepare(spec);
		delay6.prepare(spec);
		delay7.prepare(spec);
		delay8.prepare(spec);
		delay9.prepare(spec);
		delay10.prepare(spec);
		
		dampingFilter.prepare(spec);
		dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
	}
	
	//==============================================================================
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
	{
		juce::ScopedNoDenormals noDenormals;
		
		juce::dsp::AudioBlock<float> dryBlock { buffer };
		dryWetMixer.setWetMixProportion(mDryWetMix);
		dryWetMixer.pushDrySamples(dryBlock);
		
		float samplesPerMs = getSampleRate() / 1000;
		
		
		
		dampingFilter.setCutoffFrequency(mDampingCutoff);
		
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		{
			auto* channelData = buffer.getWritePointer(channel);
			
			for (int sample = 0; sample < buffer.getNumSamples(); ++channel)
			{
				
			}
		}
		
		juce::dsp::AudioBlock<float> wetBlock { buffer };
		dryWetMixer.mixWetSamples(wetBlock);
	}
	
	void reset() override
	{
		dryWetMixer.reset();
		
		delay1.reset();
		delay2.reset();
		delay3.reset();
		delay4.reset();
		delay5.reset();
		delay6.reset();
		delay7.reset();
		delay8.reset();
		delay9.reset();
		delay10.reset();
		
		dampingFilter.reset();
	}
	
	//==============================================================================
	const juce::String getName() const override { return "GardnerMediumRoom"; }
	
	//==============================================================================
	void setSize(float newSize) override { mSize = newSize; }
	void setDecay(float newDecay) override { mDecay = newDecay; }
	void setDampingCutoff(float newCutoff) override { mDampingCutoff = newCutoff; }
	void setDiffusion(float newDiffusion) override { mDiffusion = newDiffusion; }
	void setPreDelay(float newPreDelay) override { mPreDelayTime = newPreDelay; }
	void setEarlyLateMix(float newMix) override { mEarlyLateMix = newMix; }
	void setDryWetMix(float newMix) override { mDryWetMix = newMix; }
	
private:
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
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
};

//==============================================================================
class GardnerLargePlate : public ProcessorBase
{
public:
	GardnerLargePlate() {}
	
	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		// prepare stereo processors
		juce::dsp::ProcessSpec spec;
		spec.sampleRate = sampleRate;
		spec.maximumBlockSize = samplesPerBlock;
		spec.numChannels = getMainBusNumInputChannels();
		
		dryWetMixer.prepare(spec);
		
		delay1.prepare(spec);
		delay2.prepare(spec);
		delay3.prepare(spec);
		delay4.prepare(spec);
		delay5.prepare(spec);
		delay6.prepare(spec);
		delay7.prepare(spec);
		delay8.prepare(spec);
		delay9.prepare(spec);
		delay10.prepare(spec);
		delay11.prepare(spec);
		
		dampingFilter.prepare(spec);
		dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
	}
	
	//==============================================================================
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
	{
		juce::ScopedNoDenormals noDenormals;
		
		juce::dsp::AudioBlock<float> dryBlock { buffer };
		dryWetMixer.setWetMixProportion(mDryWetMix);
		dryWetMixer.pushDrySamples(dryBlock);
		
		float samplesPerMs = getSampleRate() / 1000;
		
		
		
		dampingFilter.setCutoffFrequency(mDampingCutoff);
		
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		{
			auto* channelData = buffer.getWritePointer(channel);
			
			for (int sample = 0; sample < buffer.getNumSamples(); ++channel)
			{
				
			}
		}
		
		juce::dsp::AudioBlock<float> wetBlock { buffer };
		dryWetMixer.mixWetSamples(wetBlock);
	}
	
	void reset() override
	{
		dryWetMixer.reset();
		
		delay1.reset();
		delay2.reset();
		delay3.reset();
		delay4.reset();
		delay5.reset();
		delay6.reset();
		delay7.reset();
		delay8.reset();
		delay9.reset();
		delay10.reset();
		delay11.reset();
		
		dampingFilter.reset();
	}
	
	//==============================================================================
	const juce::String getName() const override { return "GardnerLargeRoom"; }
	
	//==============================================================================
	void setSize(float newSize) override { mSize = newSize; }
	void setDecay(float newDecay) override { mDecay = newDecay; }
	void setDampingCutoff(float newCutoff) override { mDampingCutoff = newCutoff; }
	void setDiffusion(float newDiffusion) override { mDiffusion = newDiffusion; }
	void setPreDelay(float newPreDelay) override { mPreDelayTime = newPreDelay; }
	void setEarlyLateMix(float newMix) override { mEarlyLateMix = newMix; }
	void setDryWetMix(float newMix) override { mDryWetMix = newMix; }
	
private:
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
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
};
