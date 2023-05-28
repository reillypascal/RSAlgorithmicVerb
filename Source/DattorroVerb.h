/*
  ==============================================================================

    DattorroVerb.h
    Created: 27 May 2023 4:18:01pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"
#include "DelayLineWithSampleAccess.h"

class DattorroPlate : public ProcessorBase
{
public:
	DattorroPlate() {}
	
	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		// prepare stereo processors
		juce::dsp::ProcessSpec spec;
		spec.sampleRate = sampleRate;
		spec.maximumBlockSize = samplesPerBlock;
		spec.numChannels = getMainBusNumInputChannels();
		
		dryWetMixer.prepare(spec);
		dryWetMixer.reset();
		
		// prepare mono processors
		juce::dsp::ProcessSpec monoSpec;
		monoSpec.sampleRate = sampleRate;
		monoSpec.maximumBlockSize = samplesPerBlock;
		monoSpec.numChannels = 1;
		
		allpass1.prepare(monoSpec);
		allpass2.prepare(monoSpec);
		allpass3.prepare(monoSpec);
		allpass4.prepare(monoSpec);
		allpass5.prepare(monoSpec);
		allpass6.prepare(monoSpec);
		
		modulatedAPF1.prepare(monoSpec);
		modulatedAPF2.prepare(monoSpec);
		
		preDelay.prepare(monoSpec);
		
		delay1.prepare(monoSpec);
		delay2.prepare(monoSpec);
		delay3.prepare(monoSpec);
		delay4.prepare(monoSpec);
		
		inputFilter.prepare(monoSpec);
		dampingFilter1.prepare(monoSpec);
		dampingFilter2.prepare(monoSpec);
	}
	
	//==============================================================================
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
	{
		// auto totalNumInputChannels = getTotalNumInputChannels();
		
		// initialize input chain parameters
		preDelay.setDelay(mPreDelayTime);
		inputFilter.setCutoffFrequency(mDampingCutoff);
		allpass1.setDelay(210 * mSize);
		allpass2.setDelay(158 * mSize);
		allpass3.setDelay(561 * mSize);
		allpass4.setDelay(410 * mSize);
		allpass5.setDelay(3931 * mSize);
		allpass6.setDelay(2664 * mSize);
		
		// break out parameters so mod can add/subtract 12 samples
		modulatedAPF1.setDelay(1343 * mSize);
		modulatedAPF2.setDelay(995* mSize);
		
		delay1.setDelay(6241 * mSize);
		delay2.setDelay(6590 * mSize);
		delay3.setDelay(4641 * mSize);
		delay4.setDelay(5505 * mSize);
		
		// dry/wet mixer — dry samples
		dryWetMixer.setWetMixProportion(mMix);
		juce::dsp::AudioBlock<float> dryBlock { buffer };
		dryWetMixer.pushDrySamples(dryBlock);
		
		// mono reverb processing
		juce::AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
		monoBuffer.clear();
		// sum stereo to mono for input chain
		monoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
		if (buffer.getNumChannels() > 1)
		{
			monoBuffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
			monoBuffer.applyGain(0.707f);
		}
		
		// reverb sample loop params
		int channel = 0;
		// need separate ones for different delays
		float allpassFeedbackCoefficient = 0.5;
		auto* channelData = monoBuffer.getWritePointer (channel);
		for (int sample = 0; sample < monoBuffer.getNumSamples(); ++sample)
		{
			
			// apply predelay, filter
			preDelay.pushSample(channel, channelData[sample]);
			channelData[sample] = inputFilter.processSample(channel, preDelay.popSample(channel));
			
			// apply allpasses
			allpassOutput = allpass1.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			allpass1.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			allpassOutput = allpass2.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			allpass2.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			allpassOutput = allpass3.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			allpass3.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			allpassOutput = allpass4.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			allpass4.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			// figure-8 begins
			channelData[sample] += summingOutput;

			// modulated APF1
			allpassOutput = modulatedAPF1.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			modulatedAPF1.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			// delay 1
			delay1.pushSample(channel, channelData[sample]);
			channelData[sample] = dampingFilter1.processSample(channel, delay1.popSample(channel));
			
			// OUTPUT NODE A
			// L
			channel0Output = delay1.getSampleAtDelay(channel, 394 * mSize) * 0.6;
			channel0Output += delay1.getSampleAtDelay(channel, 4401 * mSize) * 0.6;
			// R
			channel1Output = -delay1.getSampleAtDelay(channel, 3124 * mSize) * 0.6;
			
			// allpass 5
			allpassOutput = allpass5.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			allpass5.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			// OUTPUT NODE B
			// L
			channel0Output -= allpass5.getSampleAtDelay(channel, 2831 * mSize) * 0.6;
			// R
			channel1Output -= allpass5.getSampleAtDelay(channel, 496 * mSize) * 0.6;
			
			//delay 2
			delay2.pushSample(channel, channelData[sample]);
			channelData[sample] = delay2.popSample(channel) * mDecay;
			
			// OUTPUT NODE C
			// L
			channel0Output += delay2.getSampleAtDelay(channel, 2954 * mSize) * 0.6;
			// R
			channel1Output -= delay2.getSampleAtDelay(channel, 179 * mSize) * 0.6;
			
			// second fig-8 half
			// modulated APF2
			allpassOutput = modulatedAPF2.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			modulatedAPF2.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			// delay 3
			delay3.pushSample(channel, channelData[sample]);
			channelData[sample] = dampingFilter2.processSample(channel, delay3.popSample(channel));
			
			// OUTPUT NODE D
			// L
			channel0Output -= delay3.getSampleAtDelay(channel, 2945 * mSize) * 0.6;
			// R
			channel1Output += delay3.getSampleAtDelay(channel, 522 * mSize) * 0.6;
			channel1Output += delay3.getSampleAtDelay(channel, 5368 * mSize) * 0.6;
			
			// allpass 6
			allpassOutput = allpass6.popSample(channel);
			feedback = allpassOutput * allpassFeedbackCoefficient;
			feedforward = -channelData[sample] - allpassOutput * allpassFeedbackCoefficient;
			allpass6.pushSample(channel, channelData[sample] + feedback);
			channelData[sample] = allpassOutput + feedforward;
			
			// OUTPUT NODE E
			// L
			channel0Output -= allpass6.getSampleAtDelay(channel, 277 * mSize) * 0.6;
			// R
			channel1Output -= allpass6.getSampleAtDelay(channel, 1817 * mSize) * 0.6;
			
			// delay 4
			delay4.pushSample(channel, channelData[sample]);
			summingOutput = (delay4.popSample(channel) * mDecay);
			
			// OUTPUT NODE F
			// L
			channel0Output -= delay4.getSampleAtDelay(channel, 1578 * mSize) * 0.6;
			// R
			channel1Output += delay4.getSampleAtDelay(channel, 3956 * mSize) * 0.6;
			
			for (int destChannel = 0; destChannel < buffer.getNumChannels(); ++destChannel)
			{
				if (destChannel == 0)
				{
					buffer.setSample(0, sample, channel0Output);
				}
				else if (destChannel == 1 && buffer.getNumChannels() > 1)
				{
					buffer.setSample(1, sample, channel1Output);
				}
			}
		}
		
		juce::dsp::AudioBlock<float> wetBlock { buffer };
		dryWetMixer.mixWetSamples(wetBlock);
	}
	
	//==============================================================================
	void reset() override
	{
		allpass1.reset();
		allpass2.reset();
		allpass3.reset();
		allpass4.reset();
		allpass5.reset();
		allpass6.reset();
		
		modulatedAPF1.reset();
		modulatedAPF2.reset();
		
		preDelay.reset();

		delay1.reset();
		delay2.reset();
		delay3.reset();
		delay4.reset();
		
		inputFilter.reset();
		dampingFilter1.reset();
		dampingFilter2.reset();
	}
	
	//==============================================================================
	const juce::String getName() const override { return "DattorroPlate"; }
	
	//==============================================================================
	void setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
	void setSize(float newSize) { mSize = newSize; }
	void setDecay(float newDecay) { mDecay = newDecay; }
	void setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
	void setDryWetMix(float newMix) { mMix = newMix; }
	
private:
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
	juce::dsp::DelayLine<float> preDelay {22050};
	DelayLineWithSampleAccess<float> delay1 {22050};
	DelayLineWithSampleAccess<float> delay2 {22050};
	DelayLineWithSampleAccess<float> delay3 {22050};
	DelayLineWithSampleAccess<float> delay4 {22050};
	// lowpass filters
	juce::dsp::FirstOrderTPTFilter<float> inputFilter;
	juce::dsp::FirstOrderTPTFilter<float> dampingFilter1;
	juce::dsp::FirstOrderTPTFilter<float> dampingFilter2;
	// mixers
	juce::dsp::DryWetMixer<float> dryWetMixer;
	
	float allpassOutput = 0;
	float feedback = 0;
	float feedforward = 0;
	float summingOutput = 0;
	float channel0Output = 0;
	float channel1Output = 0;
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mEarlyLateMix = 1;
	float mMix = 0.25;
};
