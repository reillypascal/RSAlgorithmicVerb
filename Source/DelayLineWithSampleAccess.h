/*
  ==============================================================================

    DelayLineWithSampleAccess.h
    Created: 27 May 2023 4:21:34pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

template <typename SampleType>
class DelayLineWithSampleAccess
{
public:
	DelayLineWithSampleAccess(int maximumDelayInSamples)
	{
		jassert (maximumDelayInSamples >= 0);
		
		mTotalSize = maximumDelayInSamples + 1 > 4 ? maximumDelayInSamples + 1 : 4;
		mDelayBuffer.setSize(static_cast<int>(mDelayBuffer.getNumChannels()), mTotalSize, false, false, false);
		mDelayBuffer.clear();
		mNumSamples = mDelayBuffer.getNumSamples();
		
	}
	
	~DelayLineWithSampleAccess()
	{
		//delete mDelayBuffer;
	}
	
	void pushSample(int channel, SampleType newValue)
	{
		mDelayBuffer.setSample(channel, mWritePosition[static_cast<size_t>(channel)], newValue);
		mWritePosition[static_cast<size_t>(channel)] = (mWritePosition[static_cast<size_t>(channel)] + 1) % mNumSamples;
	}
	
	SampleType popSample(int channel)
	{
		mReadPosition[static_cast<size_t>(channel)] = wrapInt((mWritePosition[static_cast<size_t>(channel)] - mDelayInSamples), mNumSamples);
		return mDelayBuffer.getSample(channel, mReadPosition[static_cast<size_t>(channel)]);
	}
	
	SampleType getSampleAtDelay(int channel, int delay)
	{
		return mDelayBuffer.getSample(channel, wrapInt((mWritePosition[static_cast<size_t>(channel)] - delay), mNumSamples));
	}
	
	SampleType interpolateSample(int channel)
	{
		auto index1 = mReadPosition;
	}
	
	void setDelay(int newLength)
	{
		mDelayInSamples = newLength;
	}
	
	void setSize(const int numChannels, const int newSize)
	{
		mTotalSize = newSize;
		mDelayBuffer.setSize(numChannels, mTotalSize, false, false, true);
		
		reset();
	}
	
	int getNumSamples()
	{
		return mDelayBuffer.getNumSamples();
	}
	
	void prepare(const juce::dsp::ProcessSpec& spec)
	{
		jassert(spec.numChannels > 0);
		
		mDelayBuffer.setSize(static_cast<int>(spec.numChannels), mTotalSize, false, false, true);
		
		mWritePosition.resize(spec.numChannels);
		mReadPosition.resize(spec.numChannels);
		
		v.resize(spec.numChannels);
		mSampleRate = spec.sampleRate;
		
		reset();
	}
	
	void reset()
	{
		for (auto vec : {&mWritePosition, &mReadPosition})
			std::fill (vec->begin(), vec->end(), 0);
		
		std::fill (v.begin(), v.end(), static_cast<SampleType>(0));
		
		mDelayBuffer.clear();
	}
	
	int wrapInt(int a, int b) {
		int c = a % b;
		return (c < 0) ? c + b : c;
	}
private:
	juce::AudioBuffer<SampleType> mDelayBuffer;
	std::vector<SampleType> v;
	int mNumSamples = 0;
	std::vector<int> mWritePosition, mReadPosition;
	SampleType delay = 0.0, delayFrac = 0.0;
	int mDelayInSamples {0};
	int mTotalSize = 4;
	
	double mSampleRate = 44100.0;
};
