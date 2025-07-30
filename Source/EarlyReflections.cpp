// FIR-based early reflections with 6 taps and HRTF for binaural stereo. Based on Dattorro

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
    
    // leftHRTFDelay.prepare(monoSpec);
    // rightHRTFDelay.prepare(monoSpec);
    
    // leftHRTFFilter.prepare(monoSpec);
    // rightHRTFFilter.prepare(monoSpec);
    
    hrtfDelays.resize(spec.numChannels);
    hrtfFilters.resize(spec.numChannels);
    channelOutputs.resize(spec.numChannels, 0.0);
    
    for (auto& delay : hrtfDelays)
    {
        delay.prepare(monoSpec);
        delay.setMaximumDelayInSamples(441);
    }
    
    for (auto& filter : hrtfFilters)
        filter.prepare(monoSpec);
}

void EarlyReflections::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    juce::AudioBuffer<float> monoBuffer(1, numSamples);
    monoBuffer.clear();
    
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
    if(numChannels > 1)
    {
        monoBuffer.addFrom(0, 0, buffer, 1, 0, numSamples);
        monoBuffer.applyGain(0.5f);
    }
    
    // leftHRTFDelay.setDelay(35);
    // rightHRTFDelay.setDelay(35);
    
    for (auto& delay : hrtfDelays)
        delay.setDelay(35);
    
    auto* channelData = monoBuffer.getWritePointer(0);
    for (int sample = 0; sample < numSamples; ++sample)
    {
        earlyReflectionsDelayLine.pushSample(0, channelData[sample]);
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            for (int delTime = 0; delTime < hrtfDelayTimes.size(); ++delTime)
            {
                // sum 3 left/ 3 right taps (interleaved w/ each other)
                // channel % 2 because could be many channels, but only 2 lists of delay times
                channelOutputs[channel] = earlyReflectionsDelayLine.getSampleAtDelay(0, hrtfDelayTimes[channel % 2][delTime] * parameters.roomSize) * initialLevel * pow(parameters.decayTime, delTime);
            }
        }
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            if (!monoFlag)
            {            
                // right into left HRTF and vice versa
                hrtfDelays[channel].pushSample(0, channelOutputs[(channel + 1) % numChannels]);
                
                // filter HRTFs and add to outputs
                channelOutputs[channel] += hrtfFilters[channel].processSample(0, hrtfDelays[channel].popSample(0));
            }
            
            // outputs into original stereo buffer
            buffer.setSample(0, sample, channelOutputs[channel]);
        }
    }
    
    // for (int sample = 0; sample < numSamples; ++sample)
    // {
    //     earlyReflectionsDelayLine.pushSample(0, channelData[sample]);
        
    //     // sum left 3 taps
    //     channel0Output = earlyReflectionsDelayLine.getSampleAtDelay(0, 441 * parameters.roomSize) * initialLevel;
    //     channel0Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 2929 * parameters.roomSize) * initialLevel * parameters.decayTime;
    //     channel0Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 6319 * parameters.roomSize) * initialLevel * pow(parameters.decayTime, 2);
        
    //     // sum right 3 taps (interleaved w/ left)
    //     channel1Output = earlyReflectionsDelayLine.getSampleAtDelay(0, 1191 * parameters.roomSize) * initialLevel;
    //     channel1Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 3948 * parameters.roomSize) * initialLevel * parameters.decayTime;
    //     channel1Output += earlyReflectionsDelayLine.getSampleAtDelay(0, 9462 * parameters.roomSize) * initialLevel * pow(parameters.decayTime, 2);
        
    //     if (!monoFlag)
    //     {
    //         // right into left HRTF and vice versa
    //         leftHRTFDelay.pushSample(0, channel1Output);
    //         rightHRTFDelay.pushSample(0, channel0Output);
            
    //         // filter HRTFs and add to outputs
    //         channel0Output += leftHRTFFilter.processSample(0, leftHRTFDelay.popSample(0));
    //         channel1Output += rightHRTFFilter.processSample(0, rightHRTFDelay.popSample(0));
    //     }
        
    //     // outputs into original stereo buffer
    //     for (int destChannel = 0; destChannel < numChannels; ++destChannel)
    //     {
    //         if (destChannel == 0)
    //             buffer.setSample(0, sample, channel0Output);
    //         else if (destChannel == 1)
    //             buffer.setSample(1, sample, channel1Output);
    //     }
    // }
}

void EarlyReflections::reset()
{
    earlyReflectionsDelayLine.reset();
    
    for (auto& delay : hrtfDelays)
        delay.reset();
        
    for (auto& filter : hrtfFilters)
        filter.reset();
    // leftHRTFDelay.reset();
    // rightHRTFDelay.reset();
    // leftHRTFFilter.reset();
    // rightHRTFFilter.reset();
}

ReverbProcessorParameters& EarlyReflections::getParameters() { return parameters; }

void EarlyReflections::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == parameters))
    {
        parameters = params;
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 1.75f);
    }
}

void EarlyReflections::setMonoFlag(const bool newMonoFlag) { monoFlag = newMonoFlag; }
