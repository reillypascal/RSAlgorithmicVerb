/*
  ==============================================================================

    ConcertHallB.h
    Created: 4 Jun 2023 4:38:39pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"
#include "DelayLineWithSampleAccess.h"
#include "LFO.h"

class LargeConcertHallB : public ProcessorBase
{
public:
	LargeConcertHallB();
	
	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	
	//==============================================================================
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
	
	//==============================================================================
	void reset() override;
	
	//==============================================================================
	const juce::String getName() const override;
	
	//==============================================================================
	void setSize(float newSize) override;
	void setDecay(float newDecay) override;
	void setDampingCutoff(float newCutoff) override;
	void setDiffusion(float newDiffusion) override;
	void setPreDelay(float newPreDelay) override;
	void setEarlyLateMix(float newMix) override;
	void setDryWetMix(float newMix) override;
	
private:
	// filters
	
	
	// delays
	
	
	// allpasses
	
	
	// mixers
	juce::dsp::DryWetMixer<float> dryWetMixer;
	
	OscillatorParameters lfoParameters;
	SignalGenData lfoOutput;
	LFO lfo;
	
	float allpassOutputInnermost = 0;
	float allpassOutputInner = 0;
	float allpassOutputOuter = 0;
	
	float feedforwardInnermost = 0;
	float feedforwardInner = 0;
	float feedforwardOuter = 0;
	
	float feedbackInnermost = 0;
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
