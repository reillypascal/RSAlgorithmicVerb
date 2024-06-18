/*
  ==============================================================================

    Collection of FDN (feedback delay network) algorithms

  ==============================================================================
*/

#include "FDNs.h"

GeneralizedFDN::GeneralizedFDN() = default;

GeneralizedFDN::GeneralizedFDN(int order, std::string type)
{
    delayCount = order;
    
    std::map<std::string, int> typeMapping
    {
        { "Anderson", 1 },
        { "Hadamard", 2 },
        { "Householder", 3 }
    };
    
    int switchCase { 1 };
    
    auto iter = typeMapping.find(type);
    if (iter != typeMapping.end())
        switchCase = iter->second;
    
    switch (switchCase) {
        case 1: // Anderson
            feedbackMatrix = {
                { 0, 0, 0, 0, 0, 0, 1, 1 },
                { 0, 0, 0, 0, 0, 0, 1, -1 },
                { 1, 1, 0, 0, 0, 0, 0, 0 },
                { 1, -1, 0, 0, 0, 0, 0, 0 },
                { 0, 0, 1, 1, 0, 0, 0, 0 },
                { 0, 0, 1, -1, 0, 0, 0, 0 },
                { 0, 0, 0, 0, 1, 1, 0, 0 },
                { 0, 0, 0, 0, 1, -1, 0, 0 }};
            
            delayTimes = { 561, 2664, 410, 1343, 210, 3931, 158, 995 };
            
            inDelays = { 4, 6 };
            modDelays = { 1, 3 };
            
            feedbackScalar = 1 / sqrt(2.0);
            break;
        
        case 2: // Hadamard
            feedbackMatrix = {
                { 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536 },
                { 0.3536, -0.3536, 0.3536, -0.3536, 0.3536, -0.3536, 0.3536, -0.3536 },
                { 0.3536, 0.3536, -0.3536, -0.3536, 0.3536, 0.3536, -0.3536, -0.3536 },
                { 0.3536, -0.3536, -0.3536, 0.3536, 0.3536, -0.3536, -0.3536, 0.3536 },
                { 0.3536, 0.3536, 0.3536, 0.3536, -0.3536, -0.3536, -0.3536, -0.3536 },
                { 0.3536, -0.3536, 0.3536, -0.3536, -0.3536, 0.3536, -0.3536, 0.3536 },
                { 0.3536, 0.3536, -0.3536, -0.3536, -0.3536, -0.3536, 0.3536, 0.3536 },
                { 0.3536, -0.3536, -0.3536, 0.3536, -0.3536, 0.3536, 0.3536, -0.3536 }};
            
            delayTimes = { 271, 487, 823, 1487, 2003, 2719, 3203, 3923 };
            
            inDelays = { 0, 1 };
            modDelays = { 3, 7 };
            
            feedbackScalar = 1.0f;
            break;
            
        case 3: // Householder
            feedbackMatrix = {
                { 0.900750, -0.190233, -0.220427, -0.240878, -0.141037, -0.141540, -0.049417, -0.059027 },
                { -0.190233, 0.635382, -0.422492, -0.461690, -0.270325, -0.271289, -0.094718, -0.113138 },
                { -0.220427, -0.422492, 0.510449, -0.534971, -0.313231, -0.314348, -0.109751, -0.131095 },
                { -0.240878, -0.461690, -0.534971, 0.415394, -0.342293, -0.343514, -0.119934, -0.143258 },
                { -0.141037, -0.270325, -0.313231, -0.342293, 0.799584, -0.201131, -0.070223, -0.083879 },
                { -0.141540, -0.271289, -0.314348, -0.343514, -0.201131, 0.798152, -0.070473, -0.084178 },
                { -0.049417, -0.094718, -0.109751, -0.119934, -0.070223, -0.070473, 0.975395, -0.029390 },
                { -0.059027, -0.113138, -0.131095, -0.143258, -0.083879, -0.084178, -0.029390, 0.964894 }};
            
            delayTimes = { 271, 487, 823, 1487, 2003, 2719, 3203, 3923 };
            
            inDelays = { 0, 1 };
            modDelays = { 3, 7 };
            
            feedbackScalar = 1.0f;
            break;
            
        default: // Anderson
            feedbackMatrix = {
                { 0, 0, 0, 0, 0, 0, 1, 1 },
                { 0, 0, 0, 0, 0, 0, 1, -1 },
                { 1, 1, 0, 0, 0, 0, 0, 0 },
                { 1, -1, 0, 0, 0, 0, 0, 0 },
                { 0, 0, 1, 1, 0, 0, 0, 0 },
                { 0, 0, 1, -1, 0, 0, 0, 0 },
                { 0, 0, 0, 0, 1, 1, 0, 0 },
                { 0, 0, 0, 0, 1, -1, 0, 0 }};
            
            delayTimes = { 561, 2664, 410, 1343, 210, 3931, 158, 995 };
            
            inDelays = { 4, 6 };
            modDelays = { 1, 3 };
            
            feedbackScalar = 1 / sqrt(2.0);
            break;
    }
}

GeneralizedFDN::~GeneralizedFDN() = default;

void GeneralizedFDN::prepare(const juce::dsp::ProcessSpec& spec)
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
        del.setMaximumDelayInSamples(22050);
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

void GeneralizedFDN::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
                    feedbackSigMatrix[channel][delIn] += delayOutputMatrix[channel][delOut] * (feedbackMatrix[delIn][delOut] * feedbackScalar);
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

void GeneralizedFDN::reset()
{
    for (auto& del : delays)
        del.reset();
    
    for (auto& filt : dampingFilters)
        filt.reset();
}

ReverbProcessorParameters& GeneralizedFDN::getParameters() { return mParameters; }

void GeneralizedFDN::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == mParameters))
        mParameters = params;
}

//===================================================================

