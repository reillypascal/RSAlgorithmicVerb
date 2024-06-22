/*
  ==============================================================================

    Tapped delay line, Allpass classes
    Delay based on juce::dsp::DelayLine, but allows access to the underlying buffer at specified sample offsets for multiple-tap delays.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

template <typename SampleType>
class DelayLineWithSampleAccess
{
public:
    DelayLineWithSampleAccess(int maximumDelayInSamples);
    
    ~DelayLineWithSampleAccess();
    
    void pushSample(int channel, SampleType newValue);
    
    SampleType popSample(int channel);
    
    SampleType getSampleAtDelay(int channel, int delay) const;
    
    void setDelay(int newLength);
    
    void setSize(const int numChannels, const int newSize);
    
    int getNumSamples() const;
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    
    void reset();
    
    int wrapInt(int a, int b) const;
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

//============================================================================

template <typename SampleType>
class Allpass
{
public:
    Allpass();
    
    ~Allpass();
    
    void setMaximumDelayInSamples(int maxDelayInSamples);
    
    void setDelay(SampleType newDelayInSamples);
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    
    void reset();
    
    void pushSample(int channel, SampleType sample);
    
    SampleType popSample(int channel, SampleType delayInSamples=-1, bool updateReadPointer=true);
    
    void setGain(SampleType newGain);
    
private:
    juce::dsp::DelayLine<SampleType> mDelayLine { 44100 };
    
    int mDelayInSamples { 4 };
    
    SampleType mGain { 0.5 };
    
    std::vector<SampleType> drySample { };
    std::vector<SampleType> delayOutput { };
    std::vector<SampleType> feedforward { };
    std::vector<SampleType> feedback { };
    
    SampleType mSampleRate { 44100.0 };
};
