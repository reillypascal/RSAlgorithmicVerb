/*
  ==============================================================================

    ConcertHallB.cpp
    Created: 4 Jun 2023 4:38:39pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#include "ConcertHallB.h"

LargeConcertHallB::LargeConcertHallB() {}

//==============================================================================
void LargeConcertHallB::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// prepare stereo processors
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getMainBusNumInputChannels();
	
	dryWetMixer.prepare(spec);
	
	// prepare filters
	inputBandwidth.prepare(spec);
	feedbackDamping.prepare(spec);
	loopDamping.prepare(spec);
	
	// prepare delays
	inputZ.prepare(spec);
	
	// prepare mono processors
	juce::dsp::ProcessSpec monoSpec;
	monoSpec.sampleRate = sampleRate;
	monoSpec.maximumBlockSize = samplesPerBlock;
	monoSpec.numChannels = 1;
	
	// prepare filters
	allpassChorusL.prepare(monoSpec);
	allpassChorusR.prepare(monoSpec);
	
	// prepare allpasses
}

//==============================================================================
void LargeConcertHallB::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	
	// dry/wet mixer — dry samples
	dryWetMixer.setWetMixProportion(mDryWetMix);
	juce::dsp::AudioBlock<float> dryBlock { buffer };
	dryWetMixer.pushDrySamples(dryBlock);
	
	// set delays
	// filters
	inputBandwidth.setDelay(1);
	feedbackDamping.setDelay(1);
	loopDamping.setCutoffFrequency(mDampingCutoff);
	allpassChorusL.setDelay(1);
	allpassChorusR.setDelay(1);
	// delays
	inputZ.setDelay(1);
	// allpasses
	
	juce::AudioBuffer<float> reverbBuffer(1, buffer.getNumSamples());
	
	auto* reverbData = reverbBuffer.getWritePointer(0);
	
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
	{
		//======== take sample in from buffer; add to channel inputs ========
		for (int channel = 0; channel < (buffer.getNumChannels() <= 2 ? buffer.getNumChannels() : 2); ++channel)
		{
			// make always 2 loops and clamp when appropriate
			// i.e. duplicate mono input into 2 parts of reverb loop
			// input
			channelInput.at(channel) = inputBandwidth.popSample(channel);
			// IN FROM BUFFER
			inputBandwidth.pushSample(channel, (buffer.getSample(channel, sample) * 0.812) + (channelInput.at(channel) * 0.188));
			inputZ.pushSample(channel, channelInput.at(channel));
			channelInput.at(channel) = inputZ.popSample(channel) * 0.5;
			
			// feedback filtering, mix of feedback/input
			channelInput.at(channel) += channelFeedback.at(channel) * (mDecay * 0.688);
			channelFeedback.at(channel) *= 0.125;
			channelFeedback.at(channel) += feedbackDamping.popSample(channel) * 0.875;
			feedbackDamping.pushSample(0, channelFeedback.at(channel));
			channelInput.at(channel) += channelFeedback.at(channel) * (mDecay * 0.312);
			
			// damping
			channelInput.at(channel) = loopDamping.processSample(channel, channelInput.at(channel));
		}
		
		//======== reverb fig-8, forking in from channelInput ========
		
		
		for (int destChannel = 0; destChannel < buffer.getNumChannels(); ++destChannel)
		{
			if (destChannel < 2)
			{
				buffer.setSample(destChannel, sample, channelOutput.at(destChannel));
			}
		}
	}
	
	juce::dsp::AudioBlock<float> wetBlock { buffer };
	// output filtering
	
	dryWetMixer.mixWetSamples(wetBlock);
}

//==============================================================================
void LargeConcertHallB::reset()
{
	
	dryWetMixer.reset();
}

//==============================================================================
const juce::String LargeConcertHallB::getName() const { return "LargeConcertHallB"; }

//==============================================================================
void LargeConcertHallB::setSize(float newSize) { mSize = newSize; }
void LargeConcertHallB::setDecay(float newDecay) { mDecay = newDecay; }
void LargeConcertHallB::setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
void LargeConcertHallB::setDiffusion(float newDiffusion) { mDiffusion = newDiffusion; }
void LargeConcertHallB::setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
void LargeConcertHallB::setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
void LargeConcertHallB::setDryWetMix(float newMix) { mDryWetMix = newMix; }
