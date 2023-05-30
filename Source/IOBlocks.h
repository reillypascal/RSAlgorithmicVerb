/*
  ==============================================================================

    IOBlocks.h
    Created: 30 May 2023 1:46:25pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"

class OutputBlock : public ProcessorBase
{
public:
	OutputBlock() {}
	
	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		juce::dsp::ProcessSpec spec;
		spec.sampleRate = sampleRate;
		spec.maximumBlockSize = samplesPerBlock;
		spec.numChannels = getMainBusNumInputChannels();
		
		lowCutFilter.prepare(spec);
		lowCutFilter.reset();
		lowCutFilter.setType(juce::dsp::FirstOrderTPTFilterType::highpass);
		highCutFilter.prepare(spec);
		highCutFilter.reset();
		highCutFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
	}
	
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
	{
		juce::ScopedNoDenormals noDenormals;
		auto totalNumInputChannels = getTotalNumInputChannels();
		auto totalNumOutputChannels = getTotalNumOutputChannels();
		
		for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
			buffer.clear (i, 0, buffer.getNumSamples());
		
		// filtering
		lowCutFilter.setCutoffFrequency(mLowCut);
		highCutFilter.setCutoffFrequency(mHighCut);
		
		juce::dsp::AudioBlock<float> block { buffer };
		
		lowCutFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
		highCutFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
	}
	
	void setLowCut(float newCutoff) { mLowCut = newCutoff; }
	void setHighCut(float newCutoff) { mHighCut = newCutoff; }
	
private:
	juce::dsp::FirstOrderTPTFilter<float> lowCutFilter;
	juce::dsp::FirstOrderTPTFilter<float> highCutFilter;
	
	float mLowCut = 0;
	float mHighCut = 20000;
};
