/*
  ==============================================================================

    Base class for reverbs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class ReverbProcessorBase
{
public:
    ReverbProcessorBase() {}
    
    virtual ~ReverbProcessorBase() {}
    
    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    
    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) = 0;
    
    virtual void reset() = 0;
    
    virtual ReverbProcessorParameters& getParameters() = 0;
    
    virtual void setParameters(const ReverbProcessorParameters& params) = 0;
};

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
	
	float scale(float input, float inLow, float inHi, float outLow, float outHi)
	{
		float scaleFactor = (outHi - outLow)/(inHi - inLow);
		float offset = outLow - inLow;
		return (input * scaleFactor) + offset;
	}
	
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
