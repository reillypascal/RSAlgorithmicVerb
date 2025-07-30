// Collection of FDN (feedback delay network) algorithms

#include "FDNs.h"
#include <cstddef>

GeneralizedFDN::GeneralizedFDN() = default;

GeneralizedFDN::GeneralizedFDN(int order, std::string type)
{
    delayCount = order;
    
    std::map<std::string, int> typeMapping
    {
        { "Anderson", 1 },
        { "circulant", 2 },
        { "Hadamard", 3 },
        { "Householder", 4 }
    };
    
    int switchCase = 1;
    
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
            
            feedbackScalar = 1.0f / sqrt(2.0f);
            break;
        
        case 2: // circulant
            feedbackMatrix = {
                { -0.068631, -0.362130, 0.107569, 0.317339, -0.336464, 0.569046, 0.297526, 0.475744 },
                { 0.475744, -0.068631, -0.362130, 0.107569, 0.317339, -0.336464, 0.569046, 0.297526 },
                { 0.297526, 0.475744, -0.068631, -0.362130, 0.107569, 0.317339, -0.336464, 0.569046 },
                { 0.569046, 0.297526, 0.475744, -0.068631, -0.362130, 0.107569, 0.317339, -0.336464 },
                { -0.336464, 0.569046, 0.297526, 0.475744, -0.068631, -0.362130, 0.107569, 0.317339 },
                { 0.317339, -0.336464, 0.569046, 0.297526, 0.475744, -0.068631, -0.362130, 0.107569 },
                { 0.107569, 0.317339, -0.336464, 0.569046, 0.297526, 0.475744, -0.068631, -0.362130 },
                { -0.362130, 0.107569, 0.317339, -0.336464, 0.569046, 0.297526, 0.475744, -0.068631 }};
            
            delayTimes = { 271, 487, 823, 1487, 2003, 2719, 3203, 3923 };
            
            inDelays = { 0, 1 };
            modDelays = { 3, 7 };
            
            feedbackScalar = 1.0f;
            break;
            
        case 3: // Hadamard
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
            
        case 4: // Householder
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
            
        default:
            // already defaults to Anderson
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
    lfoParameters.waveform = generatorWaveform::sin;
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
        lfoParameters.frequency_Hz = parameters.modRate;
        osc.setParameters(lfoParameters);
    }
    
    // set delay times
    for (size_t i = 0; i < delayCount; ++i)
        delays[i].setDelay(delayTimes[i] * parameters.roomSize);
    
    // set damping
    for (auto& filt : dampingFilters)
        filt.setCutoffFrequency(parameters.damping);
    
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
                // inDelays is input delay *indices*
                if (channel < 2 && del == inDelays[channel])
                {
                    delays[del].pushSample(channel, channelData[sample] + dampingFilters[del].processSample(channel, feedbackSigMatrix[channel][del] * parameters.decayTime));
                } else {
                    delays[del].pushSample(channel, dampingFilters[del].processSample(channel, feedbackSigMatrix[channel][del] * parameters.decayTime));
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

ReverbProcessorParameters& GeneralizedFDN::getParameters() { return parameters; }

void GeneralizedFDN::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == parameters))
    {
        parameters = params;
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 1.75f);
    }
}

//===================================================================

// unused matrices

// tiny rotation
//    feedbackMatrix = {
//        { 9.9946e-01, -2.2725e-02, 1.0834e-02, 8.8928e-03, -5.6724e-03, 9.6466e-03, 1.4271e-02, 5.3627e-03 },
//        { 2.2733e-02, 9.9949e-01, -8.4355e-03, 1.5388e-02, -1.0381e-03, 9.9210e-03, -9.0852e-03, -4.6794e-03 },
//        { -1.0854e-02, 8.4630e-03, 9.9949e-01, -1.3347e-04, 1.4917e-03, 2.1279e-02, 7.9363e-03, -1.7935e-02 },
//        { -8.8907e-03, -1.5385e-02, 1.4181e-04, 9.9948e-01, -8.7573e-03, -5.4070e-03, -1.8326e-02, -1.6668e-02 },
//        { 5.6569e-03, 1.0305e-03, -1.4764e-03, 8.7613e-03, 9.9947e-01, -1.8940e-02, 1.7530e-02, -1.6866e-02 },
//        { -9.6404e-03, -9.9042e-03, -2.1290e-02, 5.4447e-03, 1.8931e-02, 9.9947e-01, 2.8418e-03, 3.2960e-03 },
//        { -1.4253e-02, 9.1056e-03, -7.9447e-03, 1.8296e-02, -1.7559e-02, -2.8500e-03, 9.9947e-01, 7.4716e-03 },
//        { -5.3653e-03, 4.6785e-03, 1.7944e-02, 1.6682e-02, 1.6841e-02, -3.3079e-03, -7.4673e-03, 9.9950e-01 }};
