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
	
	virtual void setSize(float newSize) {}
	virtual void setDecay(float newDecay) {}
	virtual void setDampingCutoff(float newCutoff) {}
	virtual void setPreDelay(float newPreDelay) {}
	virtual void setEarlyLateMix(float newMix) {}
	virtual void setDryWetMix(float newMix) {}
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorBase)
};
