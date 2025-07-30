// Allpass loop room reverbs from Gardner, 1992

#include "GardnerRooms.h"

GardnerSmallRoom::GardnerSmallRoom() = default;

GardnerSmallRoom::~GardnerSmallRoom() = default;

void GardnerSmallRoom::prepare(const juce::dsp::ProcessSpec &spec)
{
    sampleRate = spec.sampleRate;
        
    delay1.prepare(spec);
    delay2.prepare(spec);
    delay3.prepare(spec);
    delay4.prepare(spec);
    delay5.prepare(spec);
    delay6.prepare(spec);
    
    dampingFilter.prepare(spec);
    dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    
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

void GardnerSmallRoom::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float samplesPerMs = sampleRate / 1000;
    
    dampingFilter.setCutoffFrequency(parameters.damping);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        lfoParameters = lfo[channel].getParameters();
        lfoParameters.frequency_Hz = parameters.modRate;
        lfo[channel].setParameters(lfoParameters);
        
        // segment 1
        delay1.setDelay(24 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        float delay2Time = 22 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]; // for modulation
        delay2.setDelay(delay2Time);
        delay3.setDelay(8.3 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        delay4.setDelay(4.7 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        // segment 2
        float delay5Time = 30 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]; // for modulation
        delay5.setDelay(delay5Time);
        delay6.setDelay(36 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
                
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // LFO
            lfoOutput = lfo[channel].renderAudioOutput();
            
            channelData[sample] += channelFeedback.at(channel);
            
            delay1.pushSample(channel, channelData[sample]);
            channelData[sample] = delay1.popSample(channel);
            
            // outer 35ms allpass
            feedforwardOuter = channelData[sample];
            allpassOutputOuter = delay4.popSample(channel);
            feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
            channelData[sample] = feedbackOuter;
            
            // inner 22ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay2.popSample(channel, delay2Time + (lfoOutput.normalOutput * 24.0f * parameters.modDepth)); // modulate
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.4)) * 0.4;
            delay2.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.4);
            
            // inner 8.3ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay3.popSample(channel);
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.6)) * 0.6;
            delay3.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.6);
            
            // finish outer 35ms allpass
            delay4.pushSample(channel, channelData[sample]);
            channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
            
            // mix into output
            channelOutput.at(channel) = channelData[sample] * 2.0; // up from 0.5
            
            // outer 66ms allpass
            feedforwardOuter = channelData[sample];
            allpassOutputOuter = delay6.popSample(channel);
            feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.1)) * 0.1;
            channelData[sample] = feedbackOuter;
            
            // inner 30ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay5.popSample(channel, delay5Time + (lfoOutput.quadPhaseOutput_pos * 24.0f * parameters.modDepth)); // modulate
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.4)) * 0.4;
            delay5.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.4);
            
            // finish outer 66ms allpass
            delay6.pushSample(channel, channelData[sample]);
            channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.1);
            
            // make channel feedback; mix output
            channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample] * parameters.decayTime);
            channelOutput.at(channel) += channelData[sample] * 2.0; // up from 0.5
            channelData[sample] = channelOutput.at(channel);
        }
    }
}

void GardnerSmallRoom::reset()
{
    delay1.reset();
    delay2.reset();
    delay3.reset();
    delay4.reset();
    delay5.reset();
    delay6.reset();
    
    dampingFilter.reset();
}

ReverbProcessorParameters& GardnerSmallRoom::getParameters() { return parameters; }

void GardnerSmallRoom::setParameters(const ReverbProcessorParameters &params)
{
    if (!(params == parameters))
    {
        parameters = params;
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 1.75f);
    }
}


//==============================================================================
GardnerMediumRoom::GardnerMediumRoom() = default;

GardnerMediumRoom::~GardnerMediumRoom() = default;

void GardnerMediumRoom::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
        
    delay1.prepare(spec);
    delay2.prepare(spec);
    delay3.prepare(spec);
    delay4.prepare(spec);
    delay5.prepare(spec);
    delay6.prepare(spec);
    delay7.prepare(spec);
    delay8.prepare(spec);
    delay9.prepare(spec);
    delay10.prepare(spec);
    
    dampingFilter.prepare(spec);
    dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    
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

void GardnerMediumRoom::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    juce::AudioBuffer<float> input2Buffer(buffer.getNumChannels(), buffer.getNumSamples());
    input2Buffer.clear();
    input2Buffer.makeCopyOf(buffer, false);
    
    float samplesPerMs = sampleRate / 1000;
    
    dampingFilter.setCutoffFrequency(parameters.damping);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* channelData2 = input2Buffer.getWritePointer(channel);
        
        lfoParameters = lfo[channel].getParameters();
        lfoParameters.frequency_Hz = parameters.modRate;
        lfo[channel].setParameters(lfoParameters);
        
        // input 1
        delay1.setDelay(8.3 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        float delay2Time = 22 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]; // for modulation
        delay2.setDelay(delay2Time);
        delay3.setDelay(4.7 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        // segment 2
        delay4.setDelay(5 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        float delay5Time = 30 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]; // for modulation
        delay5.setDelay(30 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        delay6.setDelay(67 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        // segment 3
        delay7.setDelay(15 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        // input 2
        delay8.setDelay(9.8 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        delay9.setDelay(29.2 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        // segment 5
        delay10.setDelay(108 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // LFO
            lfoOutput = lfo[channel].renderAudioOutput();
            
            channelData[sample] += channelFeedback.at(channel);
            
            // outer 35ms allpass
            feedforwardOuter = channelData[sample];
            allpassOutputOuter = delay3.popSample(channel);
            feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
            channelData[sample] = feedbackOuter;
            
            // inner 8.3ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay1.popSample(channel);
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.7)) * 0.7;
            delay1.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.7);
            
            // inner 22ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay2.popSample(channel, delay2Time + (lfoOutput.normalOutput * 24.0f * parameters.modDepth)); // modulate
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.5)) * 0.5;
            delay2.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.5);
            
            // finish outer 35ms allpass
            delay3.pushSample(channel, channelData[sample]);
            channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
            
            // output tap 1
            channelOutput.at(channel) = channelData[sample] * 1.0; // up from 0.5
            
            delay4.pushSample(channel, channelData[sample]);
            channelData[sample] = delay4.popSample(channel);
            
            // single 30ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay5.popSample(channel, delay5Time + (lfoOutput.quadPhaseOutput_pos * 24.0f * parameters.modDepth)); // modulate
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.5)) * 0.5;
            delay5.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.5);
            
            delay6.pushSample(channel, channelData[sample]);
            channelData[sample] = delay6.popSample(channel);
            
            // output tap 2
            channelOutput.at(channel) += channelData[sample] * 1.0; // up from 0.5
            
            delay7.pushSample(channel, channelData[sample]);
            channelData[sample] = delay7.popSample(channel);
            channelData[sample] *= parameters.decayTime;
            
            // second input to loop
            channelData[sample] += channelData2[sample];
            
            // outer 30ms allpass
            feedforwardOuter = channelData[sample];
            allpassOutputOuter = delay8.popSample(channel);
            feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
            channelData[sample] = feedbackOuter;
            
            // inner 9.8ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay9.popSample(channel);
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.6)) * 0.6;
            delay9.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.6);
            
            // finish outer 30ms allpass
            delay8.pushSample(channel, channelData[sample]);
            channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
            
            // output tap 3
            channelOutput.at(channel) += channelData[sample] * 1.0; // up from 0.5
            
            delay10.pushSample(channel, channelData[sample]);
            channelData[sample] = delay10.popSample(channel);
            
            channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample] * parameters.decayTime);
            
            channelData[sample] = channelOutput.at(channel);
        }
    }
}

void GardnerMediumRoom::reset() 
{
    delay1.reset();
    delay2.reset();
    delay3.reset();
    delay4.reset();
    delay5.reset();
    delay6.reset();
    delay7.reset();
    delay8.reset();
    delay9.reset();
    delay10.reset();
    
    dampingFilter.reset();
}

ReverbProcessorParameters& GardnerMediumRoom::getParameters() { return parameters; }

void GardnerMediumRoom::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == parameters))
    {
        parameters = params;
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 1.75f);
    }
}

//==============================================================================
GardnerLargeRoom::GardnerLargeRoom() = default;

GardnerLargeRoom::~GardnerLargeRoom() = default;

void GardnerLargeRoom::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
        
    delay1.prepare(spec);
    delay2.prepare(spec);
    delay3.prepare(spec);
    delay4.prepare(spec);
    delay5.prepare(spec);
    delay6.prepare(spec);
    delay7.prepare(spec);
    delay8.prepare(spec);
    delay9.prepare(spec);
    delay10.prepare(spec);
    delay11.prepare(spec);
    
    dampingFilter.prepare(spec);
    dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    
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

void GardnerLargeRoom::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float samplesPerMs = sampleRate / 1000;
    
    dampingFilter.setCutoffFrequency(parameters.damping);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        lfoParameters = lfo[channel].getParameters();
        lfoParameters.frequency_Hz = parameters.modRate;
        lfo[channel].setParameters(lfoParameters);
        
        // input 1
        delay1.setDelay(8 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        float delay2Time = 12 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]; // for modulation
        delay2.setDelay(delay2Time);
        delay3.setDelay(4 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        // segment 2
        delay4.setDelay(17 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        float delay5Time = 62 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]; // for modulation
        delay5.setDelay(delay5Time);
        delay6.setDelay(25 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        delay7.setDelay(31 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        // segment 3
        delay8.setDelay(3 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        delay9.setDelay(76 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        delay10.setDelay(30 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        delay11.setDelay(24 * samplesPerMs * parameters.roomSize + channelDelayOffset[channel]);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // LFO
            lfoOutput = lfo[channel].renderAudioOutput();
            
            channelData[sample] += channelFeedback.at(channel);
            
            // inner 8ms allpass
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay1.popSample(channel);
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.3)) * 0.3;
            delay1.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.3);
            
            // inner 12ms allpass (0.3)
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay2.popSample(channel, delay2Time + (lfoOutput.normalOutput * 24.0f * parameters.modDepth)); // modulate
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.3)) * 0.3;
            delay2.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.3);
            
            // 4ms delay
            delay3.pushSample(channel, channelData[sample]);
            channelData[sample] = delay3.popSample(channel);
            
            // output tap 1
            channelOutput.at(channel) = channelData[sample] * 0.68; // doubled from given 0.34
            
            // 17ms delay
            delay4.pushSample(channel, channelData[sample]);
            channelData[sample] = delay4.popSample(channel);
            
            // outer 87ms allpass (0.5)
            feedforwardOuter = channelData[sample];
            allpassOutputOuter = delay6.popSample(channel);
            feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.5)) * 0.5;
            channelData[sample] = feedbackOuter;
            
            // inner 62ms allpass (0.25)
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay5.popSample(channel, delay5Time + (lfoOutput.quadPhaseOutput_pos * 24.0f * parameters.modDepth)); // modulate
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
            delay5.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
            
            // finish outer 87ms allpass
            delay6.pushSample(channel, channelData[sample]);
            channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.5);
            
            // 31ms delay
            delay7.pushSample(channel, channelData[sample]);
            channelData[sample] = delay7.popSample(channel);
            
            // output tap 2
            channelOutput.at(channel) += channelData[sample] * 0.28; // doubled from given 0.14
            
            // 3ms delay
            delay8.pushSample(channel, channelData[sample]);
            channelData[sample] = delay8.popSample(channel);
            
            // outer 120ms allpass (0.5)
            feedforwardOuter = channelData[sample];
            allpassOutputOuter = delay11.popSample(channel);
            feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.5)) * 0.5;
            channelData[sample] = feedbackOuter;
            
            // inner 76ms allpass (0.25)
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay9.popSample(channel);
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
            delay9.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
            
            // inner 30ms allpass (0.25)
            feedforwardInner = channelData[sample];
            allpassOutputInner = delay10.popSample(channel);
            feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
            delay10.pushSample(channel, channelData[sample] + feedbackInner);
            channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
            
            // finish outer 120ms allpass
            delay11.pushSample(channel, channelData[sample]);
            channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.5);
            
            // output tap 3
            channelOutput.at(channel) += channelData[sample] * 0.28; // doubled from given 0.14
            
            // feedback/output
            channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample] * parameters.decayTime);
            channelData[sample] = channelOutput.at(channel);
        }
    }
}

void GardnerLargeRoom::reset()
{
    delay1.reset();
    delay2.reset();
    delay3.reset();
    delay4.reset();
    delay5.reset();
    delay6.reset();
    delay7.reset();
    delay8.reset();
    delay9.reset();
    delay10.reset();
    delay11.reset();
    
    dampingFilter.reset();
}

ReverbProcessorParameters& GardnerLargeRoom::getParameters() { return parameters; }

void GardnerLargeRoom::setParameters(const ReverbProcessorParameters &params)
{
    if (!(params == parameters))
    {
        parameters = params;
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 1.75f);
    }
}

////==============================================================================
//void GardnerSmallRoom::prepareToPlay(double sampleRate, int samplesPerBlock)
//{
//	// prepare stereo processors
//	juce::dsp::ProcessSpec spec;
//	spec.sampleRate = sampleRate;
//	spec.maximumBlockSize = samplesPerBlock;
//	spec.numChannels = getMainBusNumInputChannels();
//	
//	dryWetMixer.prepare(spec);
//	
//	delay1.prepare(spec);
//	delay2.prepare(spec);
//	delay3.prepare(spec);
//	delay4.prepare(spec);
//	delay5.prepare(spec);
//	delay6.prepare(spec);
//	
//	dampingFilter.prepare(spec);
//	dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//}

////==============================================================================
//void GardnerSmallRoom::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
//{
//	juce::ScopedNoDenormals noDenormals;
//	
//	juce::dsp::AudioBlock<float> dryBlock { buffer };
//	dryWetMixer.setWetMixProportion(mDryWetMix);
//	dryWetMixer.pushDrySamples(dryBlock);
//	
//	float samplesPerMs = getSampleRate() / 1000;
//	
//	dampingFilter.setCutoffFrequency(mDampingCutoff);
//	
//	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
//	{
//		auto* channelData = buffer.getWritePointer(channel);
//		
//		// segment 1
//		delay1.setDelay(24 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay2.setDelay(22 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay3.setDelay(8.3 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay4.setDelay(4.7 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		// segment 2
//		delay5.setDelay(30 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay6.setDelay(36 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		
//		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
//		{
//			channelData[sample] += channelFeedback.at(channel);
//			
//			delay1.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay1.popSample(channel);
//			
//			// outer 35ms allpass
//			feedforwardOuter = channelData[sample];
//			allpassOutputOuter = delay4.popSample(channel);
//			feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
//			channelData[sample] = feedbackOuter;
//			
//			// inner 22ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay2.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.4)) * 0.4;
//			delay2.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.4);
//			
//			// inner 8.3ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay3.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.6)) * 0.6;
//			delay3.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.6);
//			
//			// finish outer 35ms allpass
//			delay4.pushSample(channel, channelData[sample]);
//			channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
//			
//			// mix into output
//			channelOutput.at(channel) = channelData[sample] * 0.707;
//			
//			// outer 66ms allpass
//			feedforwardOuter = channelData[sample];
//			allpassOutputOuter = delay6.popSample(channel);
//			feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.1)) * 0.1;
//			channelData[sample] = feedbackOuter;
//			
//			// inner 30ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay5.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.4)) * 0.4;
//			delay5.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.4);
//			
//			// finish outer 66ms allpass
//			delay6.pushSample(channel, channelData[sample]);
//			channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.1);
//			
//			// make channel feedback; mix output
//			channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample] * mDecay);
//			channelOutput.at(channel) += channelData[sample] * 0.707;
//			channelData[sample] = channelOutput.at(channel);
//		}
//	}
//	
//	juce::dsp::AudioBlock<float> wetBlock { buffer };
//	dryWetMixer.mixWetSamples(wetBlock);
//}

////==============================================================================
//void GardnerSmallRoom::reset()
//{
//	   dryWetMixer.reset();
//	   
//	   delay1.reset();
//	   delay2.reset();
//	   delay3.reset();
//	   delay4.reset();
//	   delay5.reset();
//	   delay6.reset();
//	   
//	   dampingFilter.reset();
//}
//
////==============================================================================
//const juce::String GardnerSmallRoom::getName() const { return "GardnerSmallRoom"; }
//
////==============================================================================
//void GardnerSmallRoom::setSize(float newSize) { mSize = newSize; }
//void GardnerSmallRoom::setDecay(float newDecay) { mDecay = newDecay; }
//void GardnerSmallRoom::setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
//void GardnerSmallRoom::setDiffusion(float newDiffusion) { mDiffusion = newDiffusion; }
//void GardnerSmallRoom::setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
//void GardnerSmallRoom::setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
//void GardnerSmallRoom::setDryWetMix(float newMix) { mDryWetMix = newMix; }


////==============================================================================
//GardnerMediumRoom::GardnerMediumRoom() {}
//
////==============================================================================
//void GardnerMediumRoom::prepareToPlay(double sampleRate, int samplesPerBlock)
//{
//	// prepare stereo processors
//	juce::dsp::ProcessSpec spec;
//	spec.sampleRate = sampleRate;
//	spec.maximumBlockSize = samplesPerBlock;
//	spec.numChannels = getMainBusNumInputChannels();
//	
//	dryWetMixer.prepare(spec);
//	
//	delay1.prepare(spec);
//	delay2.prepare(spec);
//	delay3.prepare(spec);
//	delay4.prepare(spec);
//	delay5.prepare(spec);
//	delay6.prepare(spec);
//	delay7.prepare(spec);
//	delay8.prepare(spec);
//	delay9.prepare(spec);
//	delay10.prepare(spec);
//	
//	dampingFilter.prepare(spec);
//	dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//}

////==============================================================================
//void GardnerMediumRoom::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
//{
//	juce::ScopedNoDenormals noDenormals;
//	
//	juce::dsp::AudioBlock<float> dryBlock { buffer };
//	dryWetMixer.setWetMixProportion(mDryWetMix);
//	dryWetMixer.pushDrySamples(dryBlock);
//	
//	juce::AudioBuffer<float> input2Buffer(buffer.getNumChannels(), buffer.getNumSamples());
//	input2Buffer.clear();
//	input2Buffer.makeCopyOf(buffer, false);
//	
//	float samplesPerMs = getSampleRate() / 1000;
//	
//	dampingFilter.setCutoffFrequency(mDampingCutoff);
//	
//	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
//	{
//		auto* channelData = buffer.getWritePointer(channel);
//		auto* channelData2 = input2Buffer.getWritePointer(channel);
//		
//		// input 1
//		delay1.setDelay(8.3 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay2.setDelay(22 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay3.setDelay(4.7 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		// segment 2
//		delay4.setDelay(5 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay5.setDelay(30 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay6.setDelay(67 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		// segment 3
//		delay7.setDelay(15 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		// input 2
//		delay8.setDelay(9.8 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay9.setDelay(29.2 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		// segment 5
//		delay10.setDelay(108 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		
//		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
//		{
//			channelData[sample] += channelFeedback.at(channel);
//			
//			// outer 35ms allpass
//			feedforwardOuter = channelData[sample];
//			allpassOutputOuter = delay3.popSample(channel);
//			feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
//			channelData[sample] = feedbackOuter;
//			
//			// inner 8.3ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay1.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.7)) * 0.7;
//			delay1.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.7);
//			
//			// inner 22ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay2.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.5)) * 0.5;
//			delay2.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.5);
//			
//			// finish outer 35ms allpass
//			delay3.pushSample(channel, channelData[sample]);
//			channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
//			
//			// output tap 1
//			channelOutput.at(channel) = channelData[sample] * 0.5;
//			
//			delay4.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay4.popSample(channel);
//			
//			// single 30ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay5.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.5)) * 0.5;
//			delay5.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.5);
//			
//			delay6.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay6.popSample(channel);
//			
//			// output tap 2
//			channelOutput.at(channel) += channelData[sample] * 0.5;
//			
//			delay7.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay7.popSample(channel);
//			channelData[sample] *= mDecay;
//			
//			// second input to loop
//			channelData[sample] += channelData2[sample];
//			
//			// outer 30ms allpass
//			feedforwardOuter = channelData[sample];
//			allpassOutputOuter = delay8.popSample(channel);
//			feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.3)) * 0.3;
//			channelData[sample] = feedbackOuter;
//			
//			// inner 9.8ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay9.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.6)) * 0.6;
//			delay9.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.6);
//			
//			// finish outer 30ms allpass
//			delay8.pushSample(channel, channelData[sample]);
//			channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.3);
//			
//			// output tap 3
//			channelOutput.at(channel) += channelData[sample] * 0.5;
//			
//			delay10.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay10.popSample(channel);
//			
//			channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample] * mDecay);
//			
//			channelData[sample] = channelOutput.at(channel);
//		}
//	}
//	
//	juce::dsp::AudioBlock<float> wetBlock { buffer };
//	dryWetMixer.mixWetSamples(wetBlock);
//}

////==============================================================================
//void GardnerMediumRoom::reset()
//{
//	dryWetMixer.reset();
//	
//	delay1.reset();
//	delay2.reset();
//	delay3.reset();
//	delay4.reset();
//	delay5.reset();
//	delay6.reset();
//	delay7.reset();
//	delay8.reset();
//	delay9.reset();
//	delay10.reset();
//	
//	dampingFilter.reset();
//}

////==============================================================================
//const juce::String GardnerMediumRoom::getName() const { return "GardnerMediumRoom"; }
//
////==============================================================================
//void GardnerMediumRoom::setSize(float newSize) { mSize = newSize; }
//void GardnerMediumRoom::setDecay(float newDecay) { mDecay = newDecay; }
//void GardnerMediumRoom::setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
//void GardnerMediumRoom::setDiffusion(float newDiffusion) { mDiffusion = newDiffusion; }
//void GardnerMediumRoom::setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
//void GardnerMediumRoom::setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
//void GardnerMediumRoom::setDryWetMix(float newMix) { mDryWetMix = newMix; }


////==============================================================================
//GardnerLargeRoom::GardnerLargeRoom() {}
//
////==============================================================================
//void GardnerLargeRoom::prepareToPlay(double sampleRate, int samplesPerBlock)
//{
//	// prepare stereo processors
//	juce::dsp::ProcessSpec spec;
//	spec.sampleRate = sampleRate;
//	spec.maximumBlockSize = samplesPerBlock;
//	spec.numChannels = getMainBusNumInputChannels();
//	
//	dryWetMixer.prepare(spec);
//	
//	delay1.prepare(spec);
//	delay2.prepare(spec);
//	delay3.prepare(spec);
//	delay4.prepare(spec);
//	delay5.prepare(spec);
//	delay6.prepare(spec);
//	delay7.prepare(spec);
//	delay8.prepare(spec);
//	delay9.prepare(spec);
//	delay10.prepare(spec);
//	delay11.prepare(spec);
//	
//	dampingFilter.prepare(spec);
//	dampingFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//}

////==============================================================================
//void GardnerLargeRoom::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
//{
//	juce::ScopedNoDenormals noDenormals;
//	
//	juce::dsp::AudioBlock<float> dryBlock { buffer };
//	dryWetMixer.setWetMixProportion(mDryWetMix);
//	dryWetMixer.pushDrySamples(dryBlock);
//	
//	float samplesPerMs = getSampleRate() / 1000;
//	
//	dampingFilter.setCutoffFrequency(mDampingCutoff);
//	
//	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
//	{
//		auto* channelData = buffer.getWritePointer(channel);
//		
//		// input 1
//		delay1.setDelay(8 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay2.setDelay(12 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay3.setDelay(4 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		// segment 2
//		delay4.setDelay(17 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay5.setDelay(62 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay6.setDelay(25 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay7.setDelay(31 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		// segment 3
//		delay8.setDelay(3 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay9.setDelay(76 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay10.setDelay(30 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		delay11.setDelay(24 * samplesPerMs * mSize + channelDelayOffset[channel]);
//		
//		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
//		{
//			channelData[sample] += channelFeedback.at(channel);
//			
//			// inner 8ms allpass
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay1.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.3)) * 0.3;
//			delay1.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.3);
//			
//			// inner 12ms allpass (0.3)
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay2.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.3)) * 0.3;
//			delay2.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.3);
//			
//			// 4ms delay
//			delay3.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay3.popSample(channel);
//			
//			// output tap 1
//			channelOutput.at(channel) = channelData[sample] * 0.68;
//			
//			// 17ms delay
//			delay4.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay4.popSample(channel);
//			
//			// outer 87ms allpass (0.5)
//			feedforwardOuter = channelData[sample];
//			allpassOutputOuter = delay6.popSample(channel);
//			feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.5)) * 0.5;
//			channelData[sample] = feedbackOuter;
//			
//			// inner 62ms allpass (0.25)
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay5.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
//			delay5.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
//			
//			// finish outer 87ms allpass
//			delay6.pushSample(channel, channelData[sample]);
//			channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.5);
//			
//			// 31ms delay
//			delay7.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay7.popSample(channel);
//			
//			// output tap 2
//			channelOutput.at(channel) += channelData[sample] * 0.28;
//			
//			// 3ms delay
//			delay8.pushSample(channel, channelData[sample]);
//			channelData[sample] = delay8.popSample(channel);
//			
//			// outer 120ms allpass (0.5)
//			feedforwardOuter = channelData[sample];
//			allpassOutputOuter = delay11.popSample(channel);
//			feedbackOuter = (allpassOutputOuter + (feedforwardOuter * -0.5)) * 0.5;
//			channelData[sample] = feedbackOuter;
//			
//			// inner 76ms allpass (0.25)
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay9.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
//			delay9.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
//			
//			// inner 30ms allpass (0.25)
//			feedforwardInner = channelData[sample];
//			allpassOutputInner = delay10.popSample(channel);
//			feedbackInner = (allpassOutputInner + (feedforwardInner * -0.25)) * 0.25;
//			delay10.pushSample(channel, channelData[sample] + feedbackInner);
//			channelData[sample] = allpassOutputInner + (feedforwardInner * -0.25);
//			
//			// finish outer 120ms allpass
//			delay11.pushSample(channel, channelData[sample]);
//			channelData[sample] = allpassOutputOuter + (feedforwardOuter * -0.5);
//			
//			// output tap 3
//			channelOutput.at(channel) += channelData[sample] * 0.28;
//			
//			// feedback/output
//			channelFeedback.at(channel) = dampingFilter.processSample(channel, channelData[sample] * mDecay);
//			channelData[sample] = channelOutput.at(channel);
//		}
//	}
//	
//	juce::dsp::AudioBlock<float> wetBlock { buffer };
//	dryWetMixer.mixWetSamples(wetBlock);
//}

////==============================================================================
//void GardnerLargeRoom::reset()
//{
//	dryWetMixer.reset();
//	
//	delay1.reset();
//	delay2.reset();
//	delay3.reset();
//	delay4.reset();
//	delay5.reset();
//	delay6.reset();
//	delay7.reset();
//	delay8.reset();
//	delay9.reset();
//	delay10.reset();
//	delay11.reset();
//	
//	dampingFilter.reset();
//}

////==============================================================================
//void GardnerLargeRoom::setSize(float newSize) { mSize = newSize; }
//void GardnerLargeRoom::setDecay(float newDecay) { mDecay = newDecay; }
//void GardnerLargeRoom::setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
//void GardnerLargeRoom::setDiffusion(float newDiffusion) { mDiffusion = newDiffusion; }
//void GardnerLargeRoom::setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
//void GardnerLargeRoom::setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
//void GardnerLargeRoom::setDryWetMix(float newMix) { mDryWetMix = newMix; }
