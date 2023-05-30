/*
  ==============================================================================

    ProcessorBase.h
    Created: 27 May 2023 4:17:42pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ProcessorBase : public juce::AudioProcessor
{
public:
	ProcessorBase()
	: AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo())
					  .withOutput("Output", juce::AudioChannelSet::stereo())) {}
	
	void prepareToPlay (double, int) override {}
	void releaseResources() override {}
	void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override {}
	
	juce::AudioProcessorEditor* createEditor() override { return nullptr; }
	bool hasEditor() const override { return false; }
	
	const juce::String getName() const override { return {}; }
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	double getTailLengthSeconds() const override { return 0; }
	
	int getNumPrograms() override { return 0; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const juce::String getProgramName(int) override { return {}; }
	void changeProgramName(int, const juce::String&) override {}
	
	void getStateInformation(juce::MemoryBlock&) override {}
	void setStateInformation(const void*, int) override {}
	
	virtual void setSize(float newSize) { mSize = newSize; }
	virtual void setDecay(float newDecay) { mDecay = newDecay; }
	virtual void setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
	virtual void setDiffusion(float newDiffusion) { mDiffusion = newDiffusion; }
	virtual void setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
	virtual void setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
	virtual void setDryWetMix(float newMix) { mDryWetMix = newMix; }
	
private:
	float mSize = 1;
	float mDecay = 0.25;
	float mDampingCutoff = 6500;
	float mDiffusion = 0.75;
	float mPreDelayTime = 441;
	float mEarlyLateMix = 1;
	float mDryWetMix = 0.25;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorBase)
};
