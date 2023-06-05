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
	dryWetMixer.reset();
	
	// prepare mono processors
	juce::dsp::ProcessSpec monoSpec;
	monoSpec.sampleRate = sampleRate;
	monoSpec.maximumBlockSize = samplesPerBlock;
	monoSpec.numChannels = 1;
	
	// prepare filters
	inputDampingL.prepare(monoSpec);
	feedbackDampingL.prepare(monoSpec);
	loopDampingL.prepare(monoSpec);
	allpassChorusL.prepare(monoSpec);
	
	inputDampingR.prepare(monoSpec);
	feedbackDampingR.prepare(monoSpec);
	loopDampingR.prepare(monoSpec);
	allpassChorusR.prepare(monoSpec);
	
	// prepare delays
	inputZL.prepare(monoSpec);
	inputZR.prepare(monoSpec);
	
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
	
	// reverb loop buffer
	juce::AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
	monoBuffer.clear();
	
	// set delays
	// filters
	inputDampingL.setDelay(1);
	feedbackDampingL.setDelay(1);
	loopDampingL.setDelay(1);
	allpassChorusL.setDelay(1);
	
	inputDampingR.setDelay(1);
	feedbackDampingR.setDelay(1);
	loopDampingR.setDelay(1);
	allpassChorusR.setDelay(1);
	// delays
	inputZL.setDelay(1);
	inputZR.setDelay(1);
	// allpasses
	
	auto* channelDataL = buffer.getWritePointer(0);
	auto* channelDataR = buffer.getWritePointer(1);
	auto* reverbData = monoBuffer.getWritePointer(0);
	
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
	{
		// L input
		channelInput.at(0) = (channelDataL[sample] * 0.812) + (inputDampingL.popSample(0) * 0.188);
		inputDampingL.pushSample(0, channelInput.at(0));
		inputZL.pushSample(0, channelInput.at(0));
		channelInput.at(0) = inputZL.popSample(0) * 0.5;
		
		// L feedback filtering
		
		// mix of feedback/L input
		//reverbData[sample] = 
		
		// R input
		channelInput.at(1) = (channelDataR[sample] * 0.812) + (inputDampingR.popSample(0) * 0.188);
		inputDampingR.pushSample(0, channelInput.at(1));
		inputZR.pushSample(0, channelInput.at(1));
		channelInput.at(1) = inputZR.popSample(0) * 0.5;
		
		// R feedback filtering
		
		// mix of feedback/R input
		//reverbData[sample] =
		
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
