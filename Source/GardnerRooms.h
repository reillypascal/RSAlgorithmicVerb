/*
  ==============================================================================

	Allpass loop rooms from Gardner 1992

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
		
		dampingFilter.setCutoffFrequency(mDampingCutoff);
		
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		{
			auto* channelData = buffer.getWritePointer(channel);
			
			// segment 1
			delay1.setDelay(24 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay2.setDelay(22 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay3.setDelay(8.3 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay4.setDelay(4.7 * samplesPerMs * mSize + channelDelayOffset[channel]);
			// segment 2
			delay5.setDelay(30 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay6.setDelay(36 * samplesPerMs * mSize + channelDelayOffset[channel]);
			
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
	
	std::vector<float> channelDelayOffset {0, 7};
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
};

//==============================================================================
class GardnerMediumRoom : public ProcessorBase
{
public:
	GardnerMediumRoom() {}
	
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
		
		juce::AudioBuffer<float> input2Buffer(buffer.getNumChannels(), buffer.getNumSamples());
		input2Buffer.clear();
		input2Buffer.makeCopyOf(buffer, false);
		
		float samplesPerMs = getSampleRate() / 1000;
		
		dampingFilter.setCutoffFrequency(mDampingCutoff);
		
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		{
			auto* channelData = buffer.getWritePointer(channel);
			auto* channelData2 = input2Buffer.getWritePointer(channel);
			
			// input 1
			delay1.setDelay(8.3 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay2.setDelay(22 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay3.setDelay(4.7 * samplesPerMs * mSize + channelDelayOffset[channel]);
			// segment 2
			delay4.setDelay(5 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay5.setDelay(30 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay6.setDelay(67 * samplesPerMs * mSize + channelDelayOffset[channel]);
			// segment 3
			delay7.setDelay(15 * samplesPerMs * mSize + channelDelayOffset[channel]);
			// input 2
			delay8.setDelay(9.8 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay9.setDelay(29.2 * samplesPerMs * mSize + channelDelayOffset[channel]);
			// segment 5
			delay10.setDelay(108 * samplesPerMs * mSize + channelDelayOffset[channel]);
			
			for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
			{
				channelData[sample] += dampingFilter.processSample(channel, channelFeedback.at(channel)) * mDecay;
				
				// outer 35ms allpass
				feedforwardOuter = channelData[sample];
				allpassOutputOuter = delay3.popSample(channel);
				feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
				
				// inner 8.3ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay1.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.7)) * 0.7;
				delay1.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.7);
				
				// inner 22ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay2.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.5)) * 0.5;
				delay2.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.5);
				
				// finish outer 35ms allpass
				delay3.pushSample(channel, channelData[sample] + feedbackOuter);
				channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
				
				// output tap 1
				channelOutput.at(channel) = channelData[sample] * 0.5;
				
				delay4.pushSample(channel, channelData[sample]);
				channelData[sample] = delay4.popSample(channel);
				
				// single 30ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay5.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.5)) * 0.5;
				delay5.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.5);
				
				delay6.pushSample(channel, channelData[sample]);
				channelData[sample] = delay6.popSample(channel);
				
				// output tap 2
				channelOutput.at(channel) += channelData[sample] * 0.5;
				
				delay7.pushSample(channel, channelData[sample]);
				channelData[sample] = delay7.popSample(channel);
				channelData[sample] *= mDecay;
				
				// second input to loop
				channelData[sample] += channelData2[sample];
				
				// outer 30ms allpass
				feedforwardOuter = channelData[sample];
				allpassOutputOuter = delay8.popSample(channel);
				feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
				
				// inner 9.8ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay9.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.6)) * 0.6;
				delay9.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.6);
				
				// finish outer 30ms allpass
				delay8.pushSample(channel, channelData[sample] + feedbackOuter);
				channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
				
				// output tap 3
				channelOutput.at(channel) += channelData[sample] * 0.5;
				
				delay10.pushSample(channel, channelData[sample]);
				channelData[sample] = delay10.popSample(channel);
				
				channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample]);
				
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
	
	std::vector<float> channelDelayOffset {0, 15};
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
};

//==============================================================================
class GardnerLargeRoom : public ProcessorBase
{
public:
	GardnerLargeRoom() {}
	
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
			
			// input 1
			delay1.setDelay(8 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay2.setDelay(12 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay3.setDelay(4 * samplesPerMs * mSize + channelDelayOffset[channel]);
			// segment 2
			delay4.setDelay(17 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay5.setDelay(62 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay6.setDelay(25 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay7.setDelay(31 * samplesPerMs * mSize + channelDelayOffset[channel]);
			// segment 3
			delay8.setDelay(3 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay9.setDelay(76 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay10.setDelay(30 * samplesPerMs * mSize + channelDelayOffset[channel]);
			delay11.setDelay(24 * samplesPerMs * mSize + channelDelayOffset[channel]);
			
			for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
			{
				channelData[sample] += dampingFilter.processSample(channel, channelFeedback.at(channel)) * mDecay;
				
				// inner 8ms allpass
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay1.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.3)) * 0.3;
				delay1.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.3);
				
				// inner 12ms allpass (0.3)
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay2.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.3)) * 0.3;
				delay2.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.3);
				
				// 4ms delay
				delay3.pushSample(channel, channelData[sample]);
				channelData[sample] = delay3.popSample(channel);
				
				// output tap 1
				channelOutput.at(channel) = channelData[sample] * 0.34;
				
				// 17ms delay
				delay4.pushSample(channel, channelData[sample]);
				channelData[sample] = delay4.popSample(channel);
				
				// outer 87ms allpass (0.5)
				feedforwardOuter = channelData[sample];
				allpassOutputOuter = delay6.popSample(channel);
				feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.5)) * 0.5;
				
				// inner 62ms allpass (0.25)
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay5.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
				delay5.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
				
				// finish outer 87ms allpass
				delay6.pushSample(channel, channelData[sample] + feedbackOuter);
				channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.5);
				
				// 31ms delay
				delay7.pushSample(channel, channelData[sample]);
				channelData[sample] = delay7.popSample(channel);
				
				// output tap 2
				channelOutput.at(channel) += channelData[sample] * 0.14;
				
				// 3ms delay
				delay8.pushSample(channel, channelData[sample]);
				channelData[sample] = delay8.popSample(channel);
				
				// outer 120ms allpass (0.5)
				feedforwardOuter = channelData[sample];
				allpassOutputOuter = delay11.popSample(channel);
				feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.5)) * 0.5;
				
				// inner 76ms allpass (0.25)
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay9.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
				delay9.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
				
				// inner 30ms allpass (0.25)
				feedforwardInner = channelData[sample];
				allpassOutputInner = delay10.popSample(channel);
				feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
				delay10.pushSample(channel, channelData[sample] + feedbackInner);
				channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
				
				// finish outer 120ms allpass
				delay11.pushSample(channel, channelData[sample] + feedbackOuter);
				channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.5);
				
				// output tap 3
				channelOutput.at(channel) += channelData[sample] * 0.14;
				
				// feedback/output
				channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample]);
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
	
	std::vector<float> channelDelayOffset {0, 23};
	
	float mPreDelayTime = 441;
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
};
