/*
  ==============================================================================

    Tapped delay line, Allpass classes

  ==============================================================================
*/

#include <algorithm>
#include "CustomDelays.h"

template <typename SampleType>
DelayLineWithSampleAccess<SampleType>::DelayLineWithSampleAccess(int maximumDelayInSamples)
{
    jassert (maximumDelayInSamples >= 0);
    
    mTotalSize = maximumDelayInSamples + 1 > 4 ? maximumDelayInSamples + 1 : 4;
    mDelayBuffer.setSize(static_cast<int>(mDelayBuffer.getNumChannels()), mTotalSize, false, false, false);
    mDelayBuffer.clear();
    mNumSamples = mDelayBuffer.getNumSamples();
    
}

template <typename SampleType>
DelayLineWithSampleAccess<SampleType>::~DelayLineWithSampleAccess() {}

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::pushSample(int channel, SampleType newValue)
{
    mDelayBuffer.setSample(channel, mWritePosition[static_cast<size_t>(channel)], newValue);
    mWritePosition[static_cast<size_t>(channel)] = (mWritePosition[static_cast<size_t>(channel)] + 1) % mNumSamples;
}

template <typename SampleType>
SampleType DelayLineWithSampleAccess<SampleType>::popSample(int channel)
{
    mReadPosition[static_cast<size_t>(channel)] = wrapInt((mWritePosition[static_cast<size_t>(channel)] - mDelayInSamples), mNumSamples);
    return mDelayBuffer.getSample(channel, mReadPosition[static_cast<size_t>(channel)]);
}

template <typename SampleType>
SampleType DelayLineWithSampleAccess<SampleType>::getSampleAtDelay(int channel, int delay) const
{
    return mDelayBuffer.getSample(channel, wrapInt((mWritePosition[static_cast<size_t>(channel)] - delay), mNumSamples));
}

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::setDelay(int newLength) { mDelayInSamples = newLength; }

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::setSize(const int numChannels, const int newSize)
{
    mTotalSize = newSize;
    mDelayBuffer.setSize(numChannels, mTotalSize, false, false, true);
    
    reset();
}

template <typename SampleType>
int DelayLineWithSampleAccess<SampleType>::getNumSamples() const { return mDelayBuffer.getNumSamples(); }

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels > 0);
    
    mDelayBuffer.setSize(static_cast<int>(spec.numChannels), mTotalSize, false, false, true);
    
    mWritePosition.resize(spec.numChannels);
    mReadPosition.resize(spec.numChannels);
    
    v.resize(spec.numChannels);
    mSampleRate = spec.sampleRate;
    
    reset();
}

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::reset()
{
    for (auto vec : {&mWritePosition, &mReadPosition})
        std::fill (vec->begin(), vec->end(), 0);
    
    std::fill (v.begin(), v.end(), static_cast<SampleType>(0));
    
    mDelayBuffer.clear();
}

//============================================================================

template <typename SampleType>
Allpass<SampleType>::Allpass() = default;

template <typename SampleType>
Allpass<SampleType>::~Allpass() = default;

template <typename SampleType>
void Allpass<SampleType>::setMaximumDelayInSamples(int maxDelayInSamples)
{
    mDelayLine.setMaximumDelayInSamples(maxDelayInSamples);
}

template <typename SampleType>
void Allpass<SampleType>::setDelay(SampleType newDelayInSamples)
{
    mDelayLine.setDelay(newDelayInSamples);
}

template <typename SampleType>
void Allpass<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels > 0);
    
    mSampleRate = spec.sampleRate;
    
    mDelayLine.prepare(spec);
            
    drySample.resize(spec.numChannels);
    delayOutput.resize(spec.numChannels);
    feedforward.resize(spec.numChannels);
    feedback.resize(spec.numChannels);
    
    std::fill(drySample.begin(), drySample.end(), 0.0);
    std::fill(delayOutput.begin(), delayOutput.end(), 0.0);
    std::fill(feedforward.begin(), feedforward.end(), 0.0);
    std::fill(feedback.begin(), feedback.end(), 0.0);
    
    reset();
}

template <typename SampleType>
void Allpass<SampleType>::reset() { mDelayLine.reset(); }

template <typename SampleType>
void Allpass<SampleType>::pushSample(int channel, SampleType sample)
{
    mDelayLine.pushSample(channel, sample + feedback[channel]);
    drySample[channel] = sample;
}

template <typename SampleType>
SampleType Allpass<SampleType>::popSample(int channel, SampleType delayInSamples, bool updateReadPointer)
{
    delayOutput[channel] = mDelayLine.popSample(channel, delayInSamples, updateReadPointer);
    
    feedback[channel] = delayOutput[channel] * mGain;
    
    feedforward[channel] = -drySample[channel] - delayOutput[channel] * mGain;
    
    return delayOutput[channel] + feedforward[channel];
}

template <typename SampleType>
void Allpass<SampleType>::setGain(SampleType newGain) { mGain = std::clamp<SampleType>(newGain, 0.0, 1.0); }

//============================================================================

template class DelayLineWithSampleAccess<float>;
template class DelayLineWithSampleAccess<double>;

template class Allpass<float>;
template class Allpass<double>;
