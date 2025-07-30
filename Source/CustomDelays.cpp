// Tapped delay line, Allpass classes

#include <algorithm>
#include "CustomDelays.h"
#include "Utilities.h"

template <typename SampleType>
DelayLineWithSampleAccess<SampleType>::DelayLineWithSampleAccess(int maximumDelayInSamples)
{
    jassert (maximumDelayInSamples >= 0);
    
    totalSize = maximumDelayInSamples + 1 > 4 ? maximumDelayInSamples + 1 : 4;
    delayBuffer.setSize(static_cast<int>(delayBuffer.getNumChannels()), totalSize, false, false, false);
    delayBuffer.clear();
    numSamples = delayBuffer.getNumSamples();
    
}

template <typename SampleType>
DelayLineWithSampleAccess<SampleType>::~DelayLineWithSampleAccess() {}

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::pushSample(int channel, SampleType newValue)
{
    delayBuffer.setSample(channel, writePosition[static_cast<size_t>(channel)], newValue);
    writePosition[static_cast<size_t>(channel)] = (writePosition[static_cast<size_t>(channel)] + 1) % numSamples;
}

template <typename SampleType>
SampleType DelayLineWithSampleAccess<SampleType>::popSample(int channel)
{
    readPosition[static_cast<size_t>(channel)] = wrapInt((writePosition[static_cast<size_t>(channel)] - delayInSamples), numSamples);
    return delayBuffer.getSample(channel, readPosition[static_cast<size_t>(channel)]);
}

template <typename SampleType>
SampleType DelayLineWithSampleAccess<SampleType>::getSampleAtDelay(int channel, int delay) const
{
    return delayBuffer.getSample(channel, wrapInt((writePosition[static_cast<size_t>(channel)] - delay), numSamples));
}

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::setDelay(int newLength) { delayInSamples = newLength; }

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::setSize(const int numChannels, const int newSize)
{
    totalSize = newSize;
    delayBuffer.setSize(numChannels, totalSize, false, false, true);
    
    reset();
}

template <typename SampleType>
int DelayLineWithSampleAccess<SampleType>::getNumSamples() const { return delayBuffer.getNumSamples(); }

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels > 0);
    
    delayBuffer.setSize(static_cast<int>(spec.numChannels), totalSize, false, false, true);
    
    writePosition.resize(spec.numChannels);
    readPosition.resize(spec.numChannels);
    
    v.resize(spec.numChannels);
    sampleRate = spec.sampleRate;
    
    reset();
}

template <typename SampleType>
void DelayLineWithSampleAccess<SampleType>::reset()
{
    for (auto vec : {&writePosition, &readPosition})
        std::fill (vec->begin(), vec->end(), 0);
    
    std::fill (v.begin(), v.end(), static_cast<SampleType>(0));
    
    delayBuffer.clear();
}

//============================================================================

template <typename SampleType>
Allpass<SampleType>::Allpass() = default;

template <typename SampleType>
Allpass<SampleType>::~Allpass() = default;

template <typename SampleType>
void Allpass<SampleType>::setMaximumDelayInSamples(int maxDelayInSamples)
{
    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
}

template <typename SampleType>
void Allpass<SampleType>::setDelay(SampleType newDelayInSamples)
{
    delayLine.setDelay(newDelayInSamples);
}

template <typename SampleType>
void Allpass<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels > 0);
    
    sampleRate = spec.sampleRate;
    
    delayLine.prepare(spec);
            
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
void Allpass<SampleType>::reset() { delayLine.reset(); }

template <typename SampleType>
void Allpass<SampleType>::pushSample(int channel, SampleType sample)
{
    delayLine.pushSample(channel, sample + feedback[channel]);
    drySample[channel] = sample;
}

template <typename SampleType>
SampleType Allpass<SampleType>::popSample(int channel, SampleType delayInSamples, bool updateReadPointer)
{
    delayOutput[channel] = delayLine.popSample(channel, delayInSamples, updateReadPointer);
    
    feedback[channel] = delayOutput[channel] * gain;
    
    feedforward[channel] = -drySample[channel] - delayOutput[channel] * gain;
    
    return delayOutput[channel] + feedforward[channel];
}

template <typename SampleType>
void Allpass<SampleType>::setGain(SampleType newGain) { gain = std::clamp<SampleType>(newGain, 0.0, 1.0); }

//============================================================================

template class DelayLineWithSampleAccess<float>;
template class DelayLineWithSampleAccess<double>;

template class Allpass<float>;
template class Allpass<double>;
