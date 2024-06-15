/*
  ==============================================================================

    FDN based on "Designing Multi-Channel Reverberators" by "John Stautner and Miller Puckette

  ==============================================================================
*/

#include "FDNs.h"

Anderson8xFDN::Anderson8xFDN() = default;

Anderson8xFDN::~Anderson8xFDN() = default;

void Anderson8xFDN::prepare(const juce::dsp::ProcessSpec& spec)
{
    // initialize feedback/delay matrices to size channels x delays
    feedbackSigMatrix.resize(spec.numChannels);
    delayOutputMatrix.resize(spec.numChannels);
    for (int ch = 0; ch < spec.numChannels; ++ch)
    {
        feedbackSigMatrix[ch].resize(delayCount);
        delayOutputMatrix[ch].resize(delayCount);
        std::fill(feedbackSigMatrix[ch].begin(), feedbackSigMatrix[ch].end(), 0.0f);
        std::fill(delayOutputMatrix[ch].begin(), delayOutputMatrix[ch].end(), 0.0f);
    }
    
    // resize/prepare delays
    delays.resize(delayCount);
    for (auto& del : delays)
    {
        del.prepare(spec);
        del.setMaximumDelayInSamples(44100);
    }
    
    // resize/prepare filters
    dampingFilters.resize(delayCount);
    for (auto& filt : dampingFilters)
        filt.prepare(spec);
    
    // prepare lfo
    lfoParameters.frequency_Hz = 0.25;
    lfoParameters.waveform = generatorWaveform::kSin;
    lfo.resize(spec.numChannels);
    for (auto& osc : lfo)
    {
        osc.setParameters(lfoParameters);
        osc.reset(spec.sampleRate);
    }
    
    reset();
}

void Anderson8xFDN::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    // set LFO rate
    for (auto& osc : lfo)
    {
        lfoParameters = osc.getParameters();
        lfoParameters.frequency_Hz = mParameters.modRate;
        osc.setParameters(lfoParameters);
    }
    
    // set delay times
    for (int i = 0; i < delayCount; ++i)
        delays[i].setDelay(delayTimes[i] * mParameters.roomSize);
    
    // set damping
    for (auto& filt : dampingFilters)
        filt.setCutoffFrequency(mParameters.damping);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            lfoOutput = lfo[channel].renderAudioOutput();
            
            // reset channel feedback matrix to zeroes for next feedback
            std::fill(feedbackSigMatrix[channel].begin(), feedbackSigMatrix[channel].end(), 0.0f);
            
            // channel feedback matrix multiplication
            for (int delIn = 0; delIn < delayCount; ++delIn)
            {
                for (int delOut = 0; delOut < delayCount; ++delOut)
                {
                    // delIn = row; delOut = column
                    // can += all because feedbackSigMatrix starts with all zeroes
                    feedbackSigMatrix[channel][delIn] += delayOutputMatrix[channel][delOut] * (feedbackAndersonMatrix[delIn][delOut] / sqrt2);
                }
            }
            
            // signal/feedback into delays
            for (int del = 0; del < delayCount; ++del)
            {
                // only include input if this delay is the input for the channel
                if (channel < 2 && del == inDelays[channel])
                {
                    delays[del].pushSample(channel, channelData[sample] + dampingFilters[del].processSample(channel, feedbackSigMatrix[channel][del] * mParameters.decayTime));
                } else {
                    delays[del].pushSample(channel, dampingFilters[del].processSample(channel, feedbackSigMatrix[channel][del] * mParameters.decayTime));
                }
            }
            
            // zero-out channelData so all loops can use += without including dry signal
            channelData[sample] = 0;
            
            // delays into channelData
            for (int del = 0; del < delayCount; ++del)
            {
                // apply lfo if this delay is modulated
                float delayMod = 0;
                if (del == modDelays[0])
                    delayMod = lfoOutput.normalOutput;
                if (del == modDelays[1])
                    delayMod = lfoOutput.quadPhaseOutput_pos;
                
                // load delay outputs into output matrix for scaling/feedback
                delayOutputMatrix[channel][del] = delays[del].popSample(channel, delayTimes[del] + (16.0f * delayMod));
                
                // load delay data from matrix to buffer for plugin output
                channelData[sample] += delayOutputMatrix[channel][del];// / delayCount;
            }
        }
    }
}

void Anderson8xFDN::reset()
{
    for (auto& del : delays)
        del.reset();
    
    for (auto& filt : dampingFilters)
        filt.reset();
}

ReverbProcessorParameters& Anderson8xFDN::getParameters() { return mParameters; }

void Anderson8xFDN::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == mParameters))
        mParameters = params;
}
