/*
  ==============================================================================

    Freeverb.h
    Created: 27 May 2023 4:18:27pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"

class Freeverb : public ProcessorBase
{
public:
	Freeverb() {}
	
	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		juce::dsp::ProcessSpec spec;
		spec.sampleRate = sampleRate;
		spec.maximumBlockSize = samplesPerBlock;
		spec.numChannels = getMainBusNumInputChannels();
		
		comb0.prepare(spec);
		comb1.prepare(spec);
		comb2.prepare(spec);
		comb3.prepare(spec);
		comb4.prepare(spec);
		comb5.prepare(spec);
		comb6.prepare(spec);
		comb7.prepare(spec);
		
		dampingFilter0.prepare(spec);
		dampingFilter0.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		dampingFilter1.prepare(spec);
		dampingFilter1.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		dampingFilter2.prepare(spec);
		dampingFilter2.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		dampingFilter3.prepare(spec);
		dampingFilter3.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		dampingFilter4.prepare(spec);
		dampingFilter4.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		dampingFilter5.prepare(spec);
		dampingFilter5.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		dampingFilter6.prepare(spec);
		dampingFilter6.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		dampingFilter7.prepare(spec);
		dampingFilter7.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
		
		allpass0.prepare(spec);
		allpass1.prepare(spec);
		allpass2.prepare(spec);
		allpass3.prepare(spec);
		
		mixer.prepare(spec);
	}
	
	//==============================================================================
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
	{
		juce::ScopedNoDenormals noDenormals;
		
		juce::dsp::AudioBlock<float> block { buffer };
		mixer.setWetMixProportion(mDryWetMix);
		mixer.pushDrySamples(block);
		
		dampingFilter0.setCutoffFrequency(mDampingCutoff);
		dampingFilter1.setCutoffFrequency(mDampingCutoff);
		dampingFilter2.setCutoffFrequency(mDampingCutoff);
		dampingFilter3.setCutoffFrequency(mDampingCutoff);
		dampingFilter4.setCutoffFrequency(mDampingCutoff);
		dampingFilter5.setCutoffFrequency(mDampingCutoff);
		dampingFilter6.setCutoffFrequency(mDampingCutoff);
		dampingFilter7.setCutoffFrequency(mDampingCutoff);
		
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		{
			auto* channelData = buffer.getWritePointer (channel);
			
			comb0.setDelay(1557 * mSize + (channel * mWidth));
			comb1.setDelay(1617 * mSize + (channel * mWidth));
			comb2.setDelay(1491 * mSize + (channel * mWidth));
			comb3.setDelay(1422 * mSize + (channel * mWidth));
			comb4.setDelay(1277 * mSize + (channel * mWidth));
			comb5.setDelay(1356 * mSize + (channel * mWidth));
			comb6.setDelay(1188 * mSize + (channel * mWidth));
			comb7.setDelay(1116 * mSize + (channel * mWidth));
			
			allpass0.setDelay(225 + (channel * mWidth));
			allpass1.setDelay(556 + (channel * mWidth));
			allpass2.setDelay(441 + (channel * mWidth));
			allpass3.setDelay(341 + (channel * mWidth));
			
			// comb processing in parallel
			for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
			{
				auto currentSample = channelData[sample]/8;
				
				comb0Output = comb0.popSample(channel);
				comb0.pushSample(channel, dampingFilter0.processSample(channel, currentSample + comb0Output * (mDecay)));
				
				comb1Output = comb1.popSample(channel);
				comb1.pushSample(channel, dampingFilter1.processSample(channel, currentSample + comb1Output * (mDecay)));
				
				comb2Output = comb2.popSample(channel);
				comb2.pushSample(channel, dampingFilter2.processSample(channel, currentSample + comb2Output * (mDecay)));
				
				comb3Output = comb3.popSample(channel);
				comb3.pushSample(channel, dampingFilter3.processSample(channel, currentSample + comb3Output * (mDecay)));
				
				comb4Output = comb4.popSample(channel);
				comb4.pushSample(channel, dampingFilter4.processSample(channel, currentSample + comb4Output * (mDecay)));
				
				comb5Output = comb5.popSample(channel);
				comb5.pushSample(channel, dampingFilter5.processSample(channel, currentSample + comb5Output * (mDecay)));
				
				comb6Output = comb6.popSample(channel);
				comb6.pushSample(channel, dampingFilter6.processSample(channel, currentSample + comb6Output * (mDecay)));
				
				comb7Output = comb7.popSample(channel);
				comb7.pushSample(channel, dampingFilter7.processSample(channel, currentSample + comb7Output * (mDecay)));
				
				channelData[sample] = comb0Output + comb1Output + comb2Output + comb3Output + comb4Output + comb5Output + comb6Output + comb7Output;
			}
			
			// allpass processing in series
			float allpassFeedbackCoefficient = 0.5;
			
			for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
			{
				allpass0Output = allpass0.popSample(channel);
				feedback = allpass0Output * allpassFeedbackCoefficient;
				feedforward = -channelData[sample] - allpass0Output * allpassFeedbackCoefficient;
				allpass0.pushSample(channel, channelData[sample] + feedback);
				channelData[sample] = allpass0Output + feedforward;
				
				allpass1Output = allpass1.popSample(channel);
				feedback = allpass1Output * allpassFeedbackCoefficient;
				feedforward = -channelData[sample]  - allpass1Output * allpassFeedbackCoefficient;
				allpass1.pushSample(channel, channelData[sample] + feedback);
				channelData[sample] = allpass1Output + feedforward;
				
				allpass2Output = allpass2.popSample(channel);
				feedback = allpass2Output * allpassFeedbackCoefficient;
				feedforward = -channelData[sample] - allpass2Output * allpassFeedbackCoefficient;
				allpass2.pushSample(channel, channelData[sample] + feedback);
				channelData[sample] = allpass2Output + feedforward;
				
				allpass3Output = allpass3.popSample(channel);
				feedback = allpass3Output * allpassFeedbackCoefficient;
				feedforward = -channelData[sample] - allpass2Output * allpassFeedbackCoefficient;
				allpass3.pushSample(channel, channelData[sample] + feedback);
				channelData[sample] = allpass3Output + feedforward;
			}
		}
		
		mixer.mixWetSamples(block);
	}
	
	//==============================================================================
	void reset() override
	{
		comb0.reset();
		comb1.reset();
		comb2.reset();
		comb3.reset();
		comb4.reset();
		comb5.reset();
		comb6.reset();
		comb7.reset();
		
		dampingFilter0.reset();
		dampingFilter1.reset();
		dampingFilter2.reset();
		dampingFilter3.reset();
		dampingFilter4.reset();
		dampingFilter5.reset();
		dampingFilter6.reset();
		dampingFilter7.reset();
		
		allpass0.reset();
		allpass1.reset();
		allpass2.reset();
		allpass3.reset();
		
		mixer.reset();
	}
	
	//==============================================================================
	const juce::String getName() const override { return "Freeverb"; }
	
	//==============================================================================
	void setSize(float newSize) { mSize = newSize; }
	void setDecay(float newDecay) { mDecay = newDecay; }
	void setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
	void setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
	void setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
	void setDryWetMix(float newMix) { mDryWetMix = newMix; }
	
private:
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
	
	juce::dsp::DryWetMixer<float> mixer;
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
	float mWidth = 23;
};
