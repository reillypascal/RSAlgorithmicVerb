// Special-effect reverbs (delay/reverb hybrids, nonlinear decay times, etc.)

#include <algorithm>
#include "SpecialFX.h"

Constellation::Constellation() = default;

Constellation::~Constellation() = default;

void Constellation::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    samplesPerMs = sampleRate / 1000.0f;
    
    // 1 feedback value per channel
    channelFeedback.resize(spec.numChannels);
    std::fill(channelFeedback.begin(), channelFeedback.end(), 0.0f);
    
    delay.prepare(spec);
    dampingFilter.prepare(spec);
    dcFilter.prepare(spec);
    
    dcFilter.setType(juce::dsp::FirstOrderTPTFilterType::highpass);
    dcFilter.setCutoffFrequency(20.0f);
    
    // prepare lfo; 1 per channel
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

void Constellation::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    // 1 lfo per channel
    for (auto& osc : lfo)
    {
        lfoParameters = osc.getParameters();
        lfoParameters.frequency_Hz = parameters.modRate;
        osc.setParameters(lfoParameters);
    }
    
    dampingFilter.setCutoffFrequency(parameters.damping);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            lfoOutput = lfo[channel].renderAudioOutput();
            
            // input + damped feedback into delay
            delay.pushSample(channel, channelData[sample] + dcFilter.processSample(channel, dampingFilter.processSample(channel, channelFeedback[channel]) * parameters.decayTime));
            dampingFilter.snapToZero();
            dcFilter.snapToZero();
            
            // reset channelData, then sum outputs to channel
            channelData[sample] = 0.0f;
            for (int delOut = 0; delOut < 4; ++delOut)
            {
                channelData[sample] += delay.getSampleAtDelay(channel, channelOutDelayTimes[channel][delOut] * samplesPerMs) / 2.0f;
            }
            
            // reset feedback, then add delay taps
            channelFeedback[channel] = 0.0f;
            for (int delTime = 0; delTime < 16; ++delTime)
            {
                float selectedLfo { 0.0f };
                
                // cycle through 4 quadrature phases every 4 delays
                int lfoPhase = delTime % 4;
                if (lfoPhase == 3)
                    selectedLfo = lfoOutput.quadPhaseOutput_neg;
                else if (lfoPhase == 2)
                    selectedLfo = lfoOutput.invertedOutput;
                else if (lfoPhase == 1)
                    selectedLfo = lfoOutput.quadPhaseOutput_pos;
                else
                    selectedLfo = lfoOutput.normalOutput;
                
                // modulate from +/- 32 to +/- 64; does not turn fully off to allow for longer tails
                channelFeedback[channel] += delay.getSampleAtDelay(channel, (feedbackDelayTimes[delTime] * samplesPerMs * parameters.roomSize) + (selectedLfo * (parameters.modDepth * 32.0f + 32.0f))) / 7.0f;
            }
        }
    }
}

void Constellation::reset()
{
    delay.reset();
    dampingFilter.reset();
}

ReverbProcessorParameters& Constellation::getParameters() { return parameters; }

void Constellation::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == parameters))
    {
        parameters = params;
        // scale room for each reverb processor - accept from GUI as 0.0-1.0
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 4.0f);
    }
}

//=====================================================================================

EventHorizon::EventHorizon() = default;

EventHorizon::~EventHorizon() = default;

void EventHorizon::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    samplesPerMs = sampleRate / 1000.0f;
    
    // set number of allpasses in member variables; only 1 channel, so don't need to resize to num channels
    mainAllpasses.resize(numSeriesAllpasses);
    // 1 set of output allpasses per channel
    outAllpasses.resize(spec.numChannels);
    // set num output allpasses per channel
    for (auto& channel : outAllpasses)
        channel.resize(numOutputAllpasses);
    
    // prepare main allpasses (just 1 channel)
    for (int apf = 0; apf < numSeriesAllpasses; ++apf)
    {
        mainAllpasses[apf].prepare(spec);
        mainAllpasses[apf].setMaximumDelayInSamples(22050);
    }
    
    // prepare output allpasses over each channel
    for (int channel = 0; channel < spec.numChannels; ++channel)
    {
        for (int apf = 0; apf < numOutputAllpasses; ++apf)
        {
            outAllpasses[channel][apf].prepare(spec);
        }
    }
    
    // resize output allpass storage to num channels, fill w/ 0.0f
    outputAllpassValues.resize(spec.numChannels);
    std::fill(outputAllpassValues.begin(), outputAllpassValues.end(), 0.0f);
    
    // damping filters - 1 per channel
    dampingFilters.resize(numSeriesAllpasses);
    for (auto& filter : dampingFilters)
    {
        filter.prepare(spec);
        filter.setCutoffFrequency(20000.0f);
    }
    
    // prepare lfo
    lfoParameters.frequency_Hz = 0.25;
    lfoParameters.waveform = generatorWaveform::sin;
    lfo.setParameters(lfoParameters);
    lfo.reset(spec.sampleRate);
    
    reset();
}

void EventHorizon::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    lfoParameters = lfo.getParameters();
    lfoParameters.frequency_Hz = parameters.modRate;
    lfo.setParameters(lfoParameters);
    
    // delay times for main allpasses
    for (int apf = 0; apf < numSeriesAllpasses; ++apf)
    {
        mainAllpasses[apf].setDelay(delayTimes[apf] * parameters.roomSize);
        mainAllpasses[apf].setGain(parameters.decayTime);
    }
    
    // delay times for output allpasses over total num channels
    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int apf = 0; apf < numOutputAllpasses; ++apf)
        {
            // assumes stereo, but if more, alternates between two output delay lists
            outAllpasses[channel][apf].setDelay(outDelayTimes[channel % 2][apf] * parameters.roomSize);
            outAllpasses[channel][apf].setGain(parameters.decayTime);
        }
    }
    
    // filters are per channel; set damping amt
    for (auto& filter : dampingFilters)
        filter.setCutoffFrequency(parameters.damping);
    
    // mono buffer for single chain of allpasses
    juce::AudioBuffer<float> monoBuffer(1, numSamples);
    monoBuffer.clear();
    
    // if stereo, copy in R channel. If more, ignore channels other than 0/1
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
    if (numChannels > 1)
    {
        monoBuffer.addFrom(0, 0, buffer, 1, 0, numSamples);
        monoBuffer.applyGain(0.5f);
    }
    
    auto* monoData = monoBuffer.getWritePointer(0);
    
    // through samples 1 time - only 1 channel of main allpasses
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // LFO
        lfoOutput = lfo.renderAudioOutput();
        
//        monoData[sample] *= mInputScalar;
        
        // pass sample through each main allpass in series
        for (int apf = 0; apf < numSeriesAllpasses; ++apf)
        {
            float selectedLfo { 0.0f };
            
            // cycle through 4 quadrature phases every 4 delays
            int lfoPhase = apf % 4;
            if (lfoPhase == 3)
                selectedLfo = lfoOutput.quadPhaseOutput_neg;
            else if (lfoPhase == 2)
                selectedLfo = lfoOutput.invertedOutput;
            else if (lfoPhase == 1)
                selectedLfo = lfoOutput.quadPhaseOutput_pos;
            else
                selectedLfo = lfoOutput.normalOutput;
            
            mainAllpasses[apf].pushSample(0, monoData[sample]);
            monoData[sample] = dampingFilters[apf].processSample(0, mainAllpasses[apf].popSample(0, delayTimes[apf] + selectedLfo * 32.0f * parameters.modDepth));
        }
        
        // pass sample through each output allpass, channel by channel
        for (int channel = 0; channel < numChannels; ++channel)
        {
            outputAllpassValues[channel] = monoData[sample];
            // pass output values through allpasses in series
            for (int apf = 0; apf < numOutputAllpasses; ++apf)
            {
                float selectedLfo { 0.0f };
                
                // cycle through 4 quadrature phases every 4 delays
                int lfoPhase = apf % 4;
                if (lfoPhase == 3)
                    selectedLfo = lfoOutput.quadPhaseOutput_neg;
                else if (lfoPhase == 2)
                    selectedLfo = lfoOutput.invertedOutput;
                else if (lfoPhase == 1)
                    selectedLfo = lfoOutput.quadPhaseOutput_pos;
                else
                    selectedLfo = lfoOutput.normalOutput;
                
                outAllpasses[channel][apf].pushSample(channel, outputAllpassValues[channel]);
                outputAllpassValues[channel] = outAllpasses[channel][apf].popSample(channel, outDelayTimes[channel][apf] + selectedLfo * 32.0f * parameters.modDepth);
            }
            // output of final allpass into correct channel of main buffer
            auto* channelData = buffer.getWritePointer(channel);
            channelData[sample] = outputAllpassValues[channel] * outputScalar;
        }
    }
}

void EventHorizon::reset()
{
    for (auto& apf : mainAllpasses)
        apf.reset();
    
    for (auto& channel : outAllpasses)
        for (auto& apf : channel)
            apf.reset();
}

ReverbProcessorParameters& EventHorizon::getParameters() { return parameters; }

void EventHorizon::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == parameters))
    {
        parameters = params;
        // different for each reverb processor
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 2.5f);
        // clamp to avoid out-of-control behavior below 0.1
        parameters.decayTime = std::clamp(parameters.decayTime, 0.1f, 1.0f);
        // exponentially mapped to decay time — necessary to control very large output at low decay values
        outputScalar = scale(std::clamp(powf(parameters.decayTime, 5.0f), 0.0f, powf(0.8f, 5.0f)), 0.0f, powf(0.8f, 5.0f), 0.0005f, 0.45f) - 0.0001f;
    }
}
