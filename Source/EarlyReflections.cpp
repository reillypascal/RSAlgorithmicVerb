/*
  ==============================================================================

    EarlyReflections.cpp
    Created: 9 Jun 2024 3:48:05pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#include "EarlyReflections.h"

EarlyReflections::EarlyReflections() = default;

EarlyReflections::~EarlyReflections() = default;

void EarlyReflections::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec monoSpec;
    monoSpec.sampleRate = spec.sampleRate;
    monoSpec.maximumBlockSize = spec.maximumBlockSize;
    monoSpec.numChannels = 1;
    
    earlyReflectionsDelayLine.prepare(monoSpec);
    
    leftHRTFDelay.prepare(monoSpec);
    rightHRTFDelay.prepare(monoSpec);
    
    leftHRTFFilter.prepare(monoSpec);
    rightHRTFFilter.prepare(monoSpec);
}

void EarlyReflections::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    juce::AudioBuffer<float> monoBuffer(1, numSamples);
    monoBuffer.clear();
    
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
    if(numChannels > 1)
    {
        monoBuffer.addFrom(0, 0, buffer, 1, 0, numSamples);
        monoBuffer.applyGain(0.5f);
    }
    
    leftHRTFDelay.setDelay(35);
    rightHRTFDelay.setDelay(35);
    
    auto* channelData = monoBuffer.getWritePointer(0);
    for (int sample = 0; sample < numSamples; ++sample)
    {
        earlyReflectionsDelayLine.pushSample(0, channelData[sample]);
        
        // sum left 3 taps
        channel0Output = earlyReflectionsDelayLine.getSampleAtDelay(0, 441 * mParameters.roomSize) * mInitialLevel;
        channel0Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 2929 * mParameters.roomSize) * mInitialLevel * mParameters.decayTime;
        channel0Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 6319 * mParameters.roomSize) * mInitialLevel * pow(mParameters.decayTime, 2);
        
        // sum right 3 taps (interleaved w/ left)
        channel1Output = earlyReflectionsDelayLine.getSampleAtDelay(0, 1191 * mParameters.roomSize) * mInitialLevel;
        channel1Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 3948 * mParameters.roomSize) * mInitialLevel * mParameters.decayTime;
        channel1Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 9462 * mParameters.roomSize) * mInitialLevel * pow(mParameters.decayTime, 2);
        
        // right into left HRTF and vice versa
        leftHRTFDelay.pushSample(0, channel1Output);
        rightHRTFDelay.pushSample(0, channel0Output);
        
        // filter HRTFs and add to outputs
        channel0Output += leftHRTFFilter.processSample(0, leftHRTFDelay.popSample(0));
        channel1Output += rightHRTFFilter.processSample(0, rightHRTFDelay.popSample(0));
        
        // outputs into original stereo buffer
        for (int destChannel = 0; destChannel < numChannels; ++destChannel)
        {
            if (destChannel == 0)
                buffer.setSample(0, sample, channel0Output);
            else if (destChannel == 1)
                buffer.setSample(1, sample, channel1Output);
        }
    }
}

void EarlyReflections::reset()
{
    earlyReflectionsDelayLine.reset();
    leftHRTFDelay.reset();
    rightHRTFDelay.reset();
    leftHRTFFilter.reset();
    rightHRTFFilter.reset();
}

ReverbProcessorParameters& EarlyReflections::getParameters() { return mParameters; }

void EarlyReflections::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == mParameters))
        mParameters = params;
}
