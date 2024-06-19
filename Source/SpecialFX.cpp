/*
  ==============================================================================

    
  ==============================================================================
*/

#include "SpecialFX.h"

Constellation::Constellation() = default;

Constellation::~Constellation() = default;

void Constellation::prepare(const juce::dsp::ProcessSpec& spec)
{
    mSampleRate = spec.sampleRate;
    mSamplesPerMs = mSampleRate / 1000.0f;
    
    channelFeedback.resize(spec.numChannels);
    std::fill(channelFeedback.begin(), channelFeedback.end(), 0.0f);
    
    delay.prepare(spec);
    dampingFilter.prepare(spec);
    
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

void Constellation::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    for (auto& osc : lfo)
    {
        lfoParameters = osc.getParameters();
        lfoParameters.frequency_Hz = mParameters.modRate;
        osc.setParameters(lfoParameters);
    }
    
    dampingFilter.setCutoffFrequency(mParameters.damping);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            lfoOutput = lfo[channel].renderAudioOutput();
            
            // input + damped feedback into delay
            delay.pushSample(channel, channelData[sample] + (dampingFilter.processSample(channel, channelFeedback[channel]) * mParameters.decayTime));
            dampingFilter.snapToZero();
            
            // reset channelData, then sum outputs to channel
            channelData[sample] = 0.0f;
            for (int delOut = 0; delOut < 4; ++delOut)
            {
                channelData[sample] += delay.getSampleAtDelay(channel, channelOutDelayTimes[channel][delOut] * mSamplesPerMs) / 2.828f;
            }
            
            // reset feedback, then add delay taps
            channelFeedback[channel] = 0.0f;
            for (int delFb = 0; delFb < 16; ++delFb)
            {
                float selectedLfo { 0.0f };
                
                if (delFb % 4 == 0)
                    selectedLfo = lfoOutput.quadPhaseOutput_neg;
                else if (delFb % 3 == 0)
                    selectedLfo = lfoOutput.invertedOutput;
                else if (delFb % 2 == 0)
                    selectedLfo = lfoOutput.quadPhaseOutput_pos;
                else
                    selectedLfo = lfoOutput.normalOutput;
                
                channelFeedback[channel] += delay.getSampleAtDelay(channel, (feedbackDelayTimes[delFb] * mSamplesPerMs * mParameters.roomSize) + (selectedLfo * 32.0f)) / 13.0f;
            }
        }
    }
}

void Constellation::reset()
{
    delay.reset();
    dampingFilter.reset();
}

ReverbProcessorParameters& Constellation::getParameters() { return mParameters; }

void Constellation::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == mParameters))
        mParameters = params;
}

