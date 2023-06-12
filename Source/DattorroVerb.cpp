/*
  ==============================================================================

	Plate reverb "in the style of Griesinger" from Dattorro 1997

  ==============================================================================
*/

#include "DattorroVerb.h"

DattorroPlate::DattorroPlate() {}

//==============================================================================
void DattorroPlate::prepareToPlay(double sampleRate, int samplesPerBlock)
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
	
	lfoParameters.frequency_Hz = 0.25;
	lfoParameters.waveform = generatorWaveform::kSin;
	lfo.setParameters(lfoParameters);
    lfo.reset(getSampleRate());
}

//==============================================================================
void DattorroPlate::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	
	// initialize input chain parameters
	preDelay.setDelay(mPreDelayTime);
	inputFilter.setCutoffFrequency(13500);
	allpass1.setDelay(210 * mSize);
	allpass2.setDelay(158 * mSize);
	allpass3.setDelay(561 * mSize);
	allpass4.setDelay(410 * mSize);
	allpass5.setDelay(3931 * mSize);
	allpass6.setDelay(2664 * mSize);
	
	// break out parameters so mod can add/subtract 12 samples
	float modAPF1Delay = 1343 * mSize;
	float modAPF2Delay = 995 * mSize;
	modulatedAPF1.setDelay(modAPF1Delay);
	modulatedAPF2.setDelay(modAPF2Delay);
	
	delay1.setDelay(6241 * mSize);
	delay2.setDelay(6590 * mSize);
	delay3.setDelay(4641 * mSize);
	delay4.setDelay(5505 * mSize);
	
	// dry/wet mixer — dry samples
	dryWetMixer.setWetMixProportion(mDryWetMix);
	juce::dsp::AudioBlock<float> dryBlock { buffer };
	dryWetMixer.pushDrySamples(dryBlock);
	
	// mono reverb processing
	juce::AudioBuffer<float> monoBufferA(1, buffer.getNumSamples());
	juce::AudioBuffer<float> monoBufferB(1, buffer.getNumSamples());
	monoBufferA.clear();
	monoBufferB.clear();
	// sum stereo to mono for input chain
	monoBufferA.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
	monoBufferB.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
	if (buffer.getNumChannels() > 1)
	{
		monoBufferA.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
		monoBufferB.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
		monoBufferA.applyGain(0.5f);
		monoBufferB.applyGain(0.5f);
	}
	
	// reverb sample loop params
	int channel = 0;
	// need separate ones for different delays
	float decayDiffusion1 = 0.93;
	float decayDiffusion2 = 0.67;
	float inputDiffusion1 = 1;
	float inputDiffusion2 = 0.83;
	auto* channelDataA = monoBufferA.getWritePointer (channel);
	auto* channelDataB = monoBufferB.getWritePointer (channel);
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
	{
		// LFO
		lfoOutput = lfo.renderAudioOutput();
		
		// apply predelay, filter
		preDelay.pushSample(channel, channelDataA[sample]);
		channelDataA[sample] = inputFilter.processSample(channel, preDelay.popSample(channel));
		
		// apply allpasses
		allpassOutput = allpass1.popSample(channel);
		feedback = allpassOutput * inputDiffusion1 * mDiffusion;
		feedforward = -channelDataA[sample] - allpassOutput * inputDiffusion1 * mDiffusion;
		allpass1.pushSample(channel, channelDataA[sample] + feedback);
		channelDataA[sample] = allpassOutput + feedforward;
		
		allpassOutput = allpass2.popSample(channel);
		feedback = allpassOutput * inputDiffusion1 * mDiffusion;
		feedforward = -channelDataA[sample] - allpassOutput * inputDiffusion1 * mDiffusion;
		allpass2.pushSample(channel, channelDataA[sample] + feedback);
		channelDataA[sample] = allpassOutput + feedforward;
		
		allpassOutput = allpass3.popSample(channel);
		feedback = allpassOutput * inputDiffusion2 * mDiffusion;
		feedforward = -channelDataA[sample] - allpassOutput * inputDiffusion2 * mDiffusion;
		allpass3.pushSample(channel, channelDataA[sample] + feedback);
		channelDataA[sample] = allpassOutput + feedforward;
		
		allpassOutput = allpass4.popSample(channel);
		feedback = allpassOutput * inputDiffusion2 * mDiffusion;
		feedforward = -channelDataA[sample] - allpassOutput * inputDiffusion2 * mDiffusion;
		allpass4.pushSample(channel, channelDataA[sample] + feedback);
		channelDataA[sample] = allpassOutput + feedforward;
		
		// first fig-8 half
		channelDataA[sample] += summingB * mDecay;

		// modulated APF1
		allpassOutput = modulatedAPF1.popSample(channel, modAPF1Delay + (lfoOutput.normalOutput * 12));
		feedback = allpassOutput * decayDiffusion1 * mDiffusion;
		feedforward = -channelDataA[sample] - allpassOutput * decayDiffusion1 * mDiffusion;
		modulatedAPF1.pushSample(channel, channelDataA[sample] + feedback);
		channelDataA[sample] = allpassOutput + feedforward;
		
		// delay 1
		delay1.pushSample(channel, channelDataA[sample]);
		channelDataA[sample] = (dampingFilter1.processSample(channel, delay1.popSample(channel))) * mDecay;
		
		// OUTPUT NODE A
		// L
		channel0Output = delay1.getSampleAtDelay(channel, 394 * mSize) * 0.6;
		channel0Output += delay1.getSampleAtDelay(channel, 4401 * mSize) * 0.6;
		// R
		channel1Output = -delay1.getSampleAtDelay(channel, 3124 * mSize) * 0.6;
		
		// allpass 5
		allpassOutput = allpass5.popSample(channel);
		feedback = allpassOutput * decayDiffusion1 * mDiffusion;
		feedforward = -channelDataA[sample] - allpassOutput * decayDiffusion1 * mDiffusion;
		allpass5.pushSample(channel, channelDataA[sample] + feedback);
		channelDataA[sample] = allpassOutput + feedforward;
		
		// OUTPUT NODE B
		// L
		channel0Output -= allpass5.getSampleAtDelay(channel, 2831 * mSize) * 0.6;
		// R
		channel1Output -= allpass5.getSampleAtDelay(channel, 496 * mSize) * 0.6;
		
		//delay 2
		delay2.pushSample(channel, channelDataA[sample]);
		channelDataA[sample] = delay2.popSample(channel) * mDecay;
		
		// OUTPUT NODE C
		// L
		channel0Output += delay2.getSampleAtDelay(channel, 2954 * mSize) * 0.6;
		// R
		channel1Output -= delay2.getSampleAtDelay(channel, 179 * mSize) * 0.6;
		
		summingA = channelDataA[sample];
		
		// second fig-8 half
		channelDataB[sample] += summingA * mDecay;
		
		// modulated APF2
		allpassOutput = modulatedAPF2.popSample(channel, modAPF2Delay + (lfoOutput.quadPhaseOutput_pos * 12));
		feedback = allpassOutput * decayDiffusion2 * mDiffusion;
		feedforward = -channelDataB[sample] - allpassOutput * decayDiffusion2 * mDiffusion;
		modulatedAPF2.pushSample(channel, channelDataB[sample] + feedback);
		channelDataB[sample] = allpassOutput + feedforward;
		
		// delay 3
		delay3.pushSample(channel, channelDataB[sample]);
		channelDataB[sample] = (dampingFilter2.processSample(channel, delay3.popSample(channel))) * mDecay;
		
		// OUTPUT NODE D
		// L
		channel0Output -= delay3.getSampleAtDelay(channel, 2945 * mSize) * 0.6;
		// R
		channel1Output += delay3.getSampleAtDelay(channel, 522 * mSize) * 0.6;
		channel1Output += delay3.getSampleAtDelay(channel, 5368 * mSize) * 0.6;
		
		// allpass 6
		allpassOutput = allpass6.popSample(channel);
		feedback = allpassOutput * decayDiffusion2 * mDiffusion;
		feedforward = -channelDataB[sample] - allpassOutput * decayDiffusion2 * mDiffusion;
		allpass6.pushSample(channel, channelDataB[sample] + feedback);
		channelDataB[sample] = allpassOutput + feedforward;
		
		// OUTPUT NODE E
		// L
		channel0Output -= allpass6.getSampleAtDelay(channel, 277 * mSize) * 0.6;
		// R
		channel1Output -= allpass6.getSampleAtDelay(channel, 1817 * mSize) * 0.6;
		
		// delay 4
		delay4.pushSample(channel, channelDataB[sample]);
		channelDataB[sample] = delay4.popSample(channel);
		
		summingB = channelDataB[sample];
		
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
void DattorroPlate::reset()
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
	
	dryWetMixer.reset();
    
    lfo.reset(getSampleRate());
}

//==============================================================================
const juce::String DattorroPlate::getName() const { return "DattorroPlate"; }

//==============================================================================
void DattorroPlate::setSize(float newSize) { mSize = newSize; }
void DattorroPlate::setDecay(float newDecay) { mDecay = pow(newDecay, 2); }
void DattorroPlate::setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
void DattorroPlate::setDiffusion(float newDiffusion) { mDiffusion = newDiffusion; }
void DattorroPlate::setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
void DattorroPlate::setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
void DattorroPlate::setDryWetMix(float newMix) { mDryWetMix = newMix; }
