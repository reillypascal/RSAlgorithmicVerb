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
		
		juce::dsp::ProcessSpec monoSpec;
		monoSpec.sampleRate = sampleRate;
		monoSpec.maximumBlockSize = samplesPerBlock;
		spec.numChannels = 1;
		
		reset();
	}
	
	//==============================================================================
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
	{
		juce::ScopedNoDenormals noDenormals;
		
		float samplesPerMs = getSampleRate() / 1000;
		
		delay1.setDelay(24 * samplesPerMs * mSize);
		delay2.setDelay(22 * samplesPerMs * mSize);
		delay3.setDelay(8.3 * samplesPerMs * mSize);
		delay4.setDelay(4.7 * samplesPerMs * mSize);
		delay5.setDelay(36 * samplesPerMs * mSize);
		delay6.setDelay(30 * samplesPerMs * mSize);
		
		dampingFilter.setCutoffFrequency(mDampingCutoff);
		
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		{
			auto* channelData = buffer.getWritePointer(channel);
			
			for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
			{
				channelData[sample] += dampingFilter.processSample(channel, channelFeedback[channel]) * mDecay;
				
				delay1.pushSample(channel, channelData[sample]);
				channelData[sample] = delay1.popSample(channel);
				
				feedforwardOuter = channelData[sample] * -0.3;
				channelData[sample] += feedbackOuter;
				feedforwardInner = channelData[sample] * -0.4;
				channelData[sample] += feedbackInner;
				
				// inner 22ms allpass
				delay2.pushSample(channel, channelData[sample]);
				channelData[sample] = delay2.popSample(channel);
				
				channelData[sample] += feedforwardInner;
				feedbackInner = channelData[sample] * 0.4;
				
				
				channelFeedback[channel] += channelData[sample] * 0.5;
				channelData[sample] = channelOutput[channel];
			}
		}
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
		
		for (auto vec : {&channelFeedback, &channelOutput})
			std::fill(vec->begin(), vec->end(), 0);
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
	std::vector<float> channelFeedback;
	std::vector<float> channelOutput;
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
};
