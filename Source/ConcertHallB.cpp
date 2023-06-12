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
	
	// prepare delays
	// L
	loopDelayL1.prepare(monoSpec);
	loopDelayL2.prepare(monoSpec);
	loopDelayL3.prepare(monoSpec);
	loopDelayL4.prepare(monoSpec);
	// R
	loopDelayR1.prepare(monoSpec);
	loopDelayR2.prepare(monoSpec);
	loopDelayR3.prepare(monoSpec);
	loopDelayR4.prepare(monoSpec);
	
	// prepare allpasses
	// L
	allpassL1.prepare(monoSpec);
	allpassL2.prepare(monoSpec);
	allpassL3Inner.prepare(monoSpec);
	allpassL3Outer.prepare(monoSpec);
	allpassL4Innermost.prepare(monoSpec);
	allpassL4Inner.prepare(monoSpec);
	allpassL4Outer.prepare(monoSpec);
	// R
	allpassR1.prepare(monoSpec);
	allpassR2.prepare(monoSpec);
	allpassR3Inner.prepare(monoSpec);
	allpassR3Outer.prepare(monoSpec);
	allpassR4Innermost.prepare(monoSpec);
	allpassR4Inner.prepare(monoSpec);
	allpassR4Outer.prepare(monoSpec);
	
	lfoParameters.frequency_Hz = 0.5;
	lfoParameters.waveform = generatorWaveform::kSin;
	lfo.setParameters(lfoParameters);
    lfo.reset(getSampleRate());
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
	// L
	allpassChorusL.setDelay(1);
	// R
	allpassChorusR.setDelay(1);
	
	// delays
	inputZ.setDelay(1);
	// L
	loopDelayL1.setDelay(2 * mSize);
	loopDelayL2.setDelay(1055 * mSize);
	loopDelayL3.setDelay(344 * mSize);
	loopDelayL4.setDelay(1572 * mSize);
	// R
	loopDelayR1.setDelay(1 * mSize);
	loopDelayR2.setDelay(1460 * mSize);
	loopDelayR3.setDelay(500 * mSize);
	loopDelayR4.setDelay(16 * mSize);
	
	// allpasses
	// L
	allpassL1.setDelay(239 * mSize);
	allpassL2.setDelay(392 * mSize);
	allpassL3Inner.setDelay(1944 * mSize);
	allpassL3Outer.setDelay(612 * mSize);
	allpassL4Innermost.setDelay(1333 * mSize);
	allpassL4Inner.setDelay(819 * mSize);
	allpassL4Outer.setDelay(1264 * mSize);
	// R
	allpassR1.setDelay(205 * mSize);
	allpassR2.setDelay(329 * mSize);
	allpassR3Inner.setDelay(2032 * mSize);
	allpassR3Outer.setDelay(368 * mSize);
	allpassR4Innermost.setDelay(1457 * mSize);
	allpassR4Inner.setDelay(688 * mSize);
	allpassR4Outer.setDelay(1340 * mSize);
	
	juce::AudioBuffer<float> reverbBuffer(1, buffer.getNumSamples());
	
	auto* reverbData = reverbBuffer.getWritePointer(0);
	
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
	{
		//================ render next LFO step ================
		lfoOutput = lfo.renderAudioOutput();
		
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
			channelFeedback.at(channel) *= -0.125;
			channelFeedback.at(channel) += feedbackDamping.popSample(channel) * 0.875;
			feedbackDamping.pushSample(0, channelFeedback.at(channel));
			channelInput.at(channel) += channelFeedback.at(channel) * (mDecay * 0.312);
			
			// damping
			channelInput.at(channel) = loopDamping.processSample(channel, channelInput.at(channel));
		}
		
		//======== reverb fig-8, forking in from channelInput ========
		//================ begin L loop ================
		// allpass 1
		allpassOutputInner = allpassL1.popSample(0);
		feedbackInner = allpassOutputInner * 0.375 * mDiffusion;
		reverbData[sample] = channelInput.at(0) + feedbackInner;
		feedforwardInner = reverbData[sample] * -0.375 * mDiffusion;
		allpassL1.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.844 * mDecay) + feedforwardInner;
		
		loopDelayL1.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayL1.popSample(0);
		
		// allpass 2
		allpassOutputInner = allpassL2.popSample(0);
		feedbackInner = allpassOutputInner * 0.312 * mDiffusion;
		reverbData[sample] += feedbackInner;
		feedforwardInner = reverbData[sample] * -0.312 * mDiffusion;
		allpassL2.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.906 * mDecay) + feedforwardInner;
		// node 23
		loopDelayL2.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayL2.popSample(0);
		// start output R
		channelOutput.at(1) = reverbData[sample] * 0.938;
		
		// nested allpass 3
		// begin outer
		allpassOutputOuter = allpassL3Outer.popSample(0);
		feedbackOuter = allpassOutputOuter * 0.406 * mDiffusion;
		reverbData[sample] += feedbackOuter;
		feedforwardOuter = reverbData[sample] * -0.406 * mDiffusion;
		// inner
		allpassOutputInner = allpassL3Inner.popSample(0);
		feedbackInner = allpassOutputInner * 0.25 * mDiffusion;
		reverbData[sample] += feedbackInner;
		feedforwardInner = reverbData[sample] * -0.25 * mDiffusion;
		allpassL3Inner.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.938 * mDecay) + feedforwardInner;
		// finish outer
		allpassL3Outer.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputOuter * 0.844 * mDecay) + feedforwardOuter;
		
		// node 27_31
		loopDelayL3.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayL3.popSample(0);
		// start output L
		channelOutput.at(0) = (loopDelayL3.getSampleAtDelay(0, 276) * 0.938) - (loopDelayL3.getSampleAtDelay(0, 312) * 0.438);
		// output R
		channelOutput.at(1) += loopDelayL3.getSampleAtDelay(0, 40) * 0.438;
		
		// nested allpass 4
		// begin outer
		allpassOutputOuter = allpassL4Outer.popSample(0);
		feedbackOuter = allpassOutputOuter * 0.406 * mDiffusion;
		reverbData[sample] += feedbackOuter;
		feedforwardOuter = reverbData[sample] * -0.406 * mDiffusion;
		// begin inner
		allpassOutputInner = allpassL4Inner.popSample(0);
		feedbackInner = allpassOutputInner * 0.25 * mDiffusion;
		reverbData[sample] += feedbackInner;
		feedforwardInner = reverbData[sample] * -0.25 * mDiffusion;
		// innermost
		allpassOutputInnermost = allpassL4Innermost.popSample(0);
		// chorus
		allpassChorusL.pushSample(0, allpassOutputInnermost);
		allpassOutputInnermost *= 0.781;
		allpassOutputInnermost += allpassChorusL.popSample(0, scale(lfoOutput.normalOutput, -1, 1, 1, 12)) * 0.219; // modulate here
		// finish innermost
		feedbackInnermost = allpassOutputInnermost * 0.25 * mDiffusion;
		reverbData[sample] += feedbackInnermost;
		feedforwardInnermost = reverbData[sample] * -0.25 * mDiffusion;
		allpassL4Innermost.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInnermost * 0.938) + feedforwardInnermost;
		// finish inner
		allpassL4Inner.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.938 * mDecay) + feedforwardInner;
		// finish outer
		allpassL4Outer.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputOuter * 0.844 * mDecay) + feedforwardOuter;
		
		// node 37_39
		loopDelayL4.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayL4.popSample(0);
		// output R
		channelOutput.at(1) += (loopDelayL4.getSampleAtDelay(0, 36) * 0.469) + (loopDelayL4.getSampleAtDelay(0, 1572) * 0.125);
		
		// feedback *TO* R channel
		channelFeedback.at(1) = reverbData[sample];
		
		//================ begin R loop ================
		// allpass 1
		allpassOutputInner = allpassR1.popSample(0);
		feedbackInner = allpassOutputInner * 0.375 * mDiffusion;
		reverbData[sample] = channelInput.at(1) + feedbackInner;
		feedforwardInner = reverbData[sample] * -0.375 * mDiffusion;
		allpassR1.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.844 * mDecay) + feedforwardInner;
		
		loopDelayR1.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayR1.popSample(0);
		
		// allpass 2
		allpassOutputInner = allpassR2.popSample(0);
		feedbackInner = allpassOutputInner * 0.312 * mDiffusion;
		reverbData[sample] += feedbackInner;
		feedforwardInner = reverbData[sample] * -0.312 * mDiffusion;
		allpassR2.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.906 * mDecay) + feedforwardInner;
		
		// node 40_42
		loopDelayR2.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayR2.popSample(0);
		// output L
		channelOutput.at(0) += loopDelayR2.getSampleAtDelay(0, 625) * 0.938;
		
		// nested allpass 3
		// begin outer
		allpassOutputOuter = allpassR3Outer.popSample(0);
		feedbackOuter = allpassOutputOuter * 0.406 * mDiffusion;
		reverbData[sample] += feedbackOuter;
		feedforwardOuter = reverbData[sample] * -0.406 * mDiffusion;
		// inner
		allpassOutputInner = allpassR3Inner.popSample(0);
		feedbackInner = allpassOutputInner * 0.25 * mDiffusion;
		reverbData[sample] += feedbackInner;
		feedforwardInner = reverbData[sample] * -0.25 * mDiffusion;
		allpassR3Inner.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.938 * mDecay) + feedforwardInner;
		// finish outer
		allpassR3Outer.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputOuter * 0.844 * mDecay) + feedforwardOuter;
		
		// node 45_49
		loopDelayR3.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayR3.popSample(0);
		// output L
		channelOutput.at(0) += loopDelayR3.getSampleAtDelay(0, 468) * 0.438;
		channelOutput.at(1) += (loopDelayR3.getSampleAtDelay(0, 24) * 0.938) - (loopDelayR3.getSampleAtDelay(0, 192) * 0.438);
		
		// allpass 4
		// begin outer
		allpassOutputOuter = allpassR4Outer.popSample(0);
		feedbackOuter = allpassOutputInner * 0.406 * mDiffusion;
		reverbData[sample] += feedbackOuter;
		feedforwardOuter = reverbData[sample] * -0.406 * mDiffusion;
		// begin inner
		allpassOutputInner = allpassR4Inner.popSample(0);
		feedbackInner = allpassOutputInner * 0.25 * mDiffusion;
		reverbData[sample] += feedbackInner;
		feedforwardInner = reverbData[sample] * -0.25 * mDiffusion;
		// innermost
		allpassOutputInnermost = allpassR4Innermost.popSample(0);
		// chorus
		allpassChorusR.pushSample(0, allpassOutputInnermost);
		allpassOutputInnermost *= 0.781;
		allpassOutputInnermost += allpassChorusL.popSample(0, scale(lfoOutput.quadPhaseOutput_pos, -1, 1, 1, 12)) * 0.219; // modulate here
		// finish innermost
		feedbackInnermost = allpassOutputInnermost * 0.25 * mDiffusion;
		reverbData[sample] += feedbackInnermost;
		feedforwardInnermost = reverbData[sample] * -0.25 * mDiffusion;
		allpassR4Innermost.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInnermost * 0.938 * mDecay) + feedforwardInnermost;
		// finish inner
		allpassR4Inner.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputInner * 0.938 * mDecay) + feedbackInner;
		// finish outer
		allpassR4Outer.pushSample(0, reverbData[sample]);
		reverbData[sample] = (allpassOutputOuter * 0.844 * mDecay) + feedforwardOuter;
		
		// node 55_58
		loopDelayR4.pushSample(0, reverbData[sample]);
		reverbData[sample] = loopDelayR4.popSample(0);
		// output L
		channelOutput.at(0) += loopDelayR4.getSampleAtDelay(0, 8) * 0.125;
		
		// feedback *TO* L channel
		channelFeedback.at(0) = reverbData[sample];
		
		//================ write to output ================
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
	// reset filters
	inputBandwidth.reset();
	feedbackDamping.reset();
	loopDamping.reset();
	// reset filters
	allpassChorusL.reset();
	allpassChorusR.reset();
	
	// reset delays
	inputZ.reset();
	// L
	loopDelayL1.reset();
	loopDelayL2.reset();
	loopDelayL3.reset();
	loopDelayL4.reset();
	// R
	loopDelayR1.reset();
	loopDelayR2.reset();
	loopDelayR3.reset();
	loopDelayR4.reset();
	
	// reset allpasses
	// L
	allpassL1.reset();
	allpassL2.reset();
	allpassL3Inner.reset();
	allpassL3Outer.reset();
	allpassL4Innermost.reset();
	allpassL4Inner.reset();
	allpassL4Outer.reset();
	// R
	allpassR1.reset();
	allpassR2.reset();
	allpassR3Inner.reset();
	allpassR3Outer.reset();
	allpassR4Innermost.reset();
	allpassR4Inner.reset();
	allpassR4Outer.reset();
	
	dryWetMixer.reset();
    
    lfo.reset(getSampleRate());
}

//==============================================================================
const juce::String LargeConcertHallB::getName() const { return "LargeConcertHallB"; }

//==============================================================================
void LargeConcertHallB::setSize(float newSize) { mSize = newSize * (44.1 / 34.125); }
void LargeConcertHallB::setDecay(float newDecay) { mDecay = scale(newDecay, 0, 1, 0.25, 1); }
void LargeConcertHallB::setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
void LargeConcertHallB::setDiffusion(float newDiffusion) { mDiffusion = newDiffusion * 1.75; }
void LargeConcertHallB::setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
void LargeConcertHallB::setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
void LargeConcertHallB::setDryWetMix(float newMix) { mDryWetMix = newMix; }
