// "Freeverb"-style Schroeder reverb; 8 parallel combs -> 4 series allpasses

#include "Freeverb.h"

Freeverb::Freeverb() = default;

Freeverb::~Freeverb() = default;

void Freeverb::prepare(const juce::dsp::ProcessSpec& spec)
{
    // 8 combs/damping filters in parallel; 4 allpasses in series
    combs.resize(combCount);
    dampingFilters.resize(combCount);
    allpasses.resize(allpassCount);
    
    for (auto& comb : combs)
    {
        comb.prepare(spec);
        comb.setMaximumDelayInSamples(spec.sampleRate * 0.5);
    }
    for (auto& filter : dampingFilters)
    {
        filter.prepare(spec);
        filter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    }
    for (auto& allpass : allpasses)
    {
        allpass.prepare(spec);
        allpass.setMaximumDelayInSamples(spec.sampleRate * 0.5);
    }
    
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

void Freeverb::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    // set LFO rate
    for (auto& osc : lfo)
    {
        lfoParameters = osc.getParameters();
        lfoParameters.frequency_Hz = parameters.modRate;
        osc.setParameters(lfoParameters);
    }
    
    // set up comb damping filters
    for (int i = 0; i < combCount; ++i)
        dampingFilters[i].setCutoffFrequency(parameters.damping);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        lfoParameters = lfo[channel].getParameters();
        lfoParameters.frequency_Hz = parameters.modRate;
        lfo[channel].setParameters(lfoParameters);
        
        // set up combs - need to be in channel loop to have channel spread
        for (int i = 0; i < combCount; ++i)
            combs[i].setDelay(combDelayTimes[i] * parameters.roomSize + (channel * stereoWidth));
        
        // set up allpasses
        for (int i = 0; i < allpassCount; ++i)
            allpasses[i].setDelay(allpassDelayTimes[i] * parameters.roomSize + (channel * stereoWidth));
        
        // comb processing in parallel
        for (int sample = 0; sample < numSamples; ++sample)
        {
//            auto currentSample = channelData[sample]/8;
            
            float combMix = 0;
            
            for (int i = 0; i < combCount; ++i)
            {
                float combOutput = combs[i].popSample(channel);
                combs[i].pushSample(channel,
                                    dampingFilters[i].processSample(channel,
                                                                    channelData[sample] + combOutput * parameters.decayTime));
                combMix += combOutput;
            }
            
            channelData[sample] = combMix / combCount;
        }
        
        // allpass processing in series
        float allpassFeedbackCoefficient = 0.5;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // LFO
            lfoOutput = lfo[channel].renderAudioOutput();
            
            for (int i = 0; i < allpassCount; ++i)
            {
                float delayOutput;
                
                if (i % 2 == 0)
                    delayOutput = allpasses[i].popSample(channel, allpassDelayTimes[i] + (lfoOutput.normalOutput * 12.0f * parameters.modDepth));
                else
                    delayOutput = allpasses[i].popSample(channel);
                
                float feedback = delayOutput * -allpassFeedbackCoefficient;
                float vn = channelData[sample] + feedback;
                allpasses[i].pushSample(channel, vn);
                channelData[sample] = delayOutput + (vn * allpassFeedbackCoefficient);
            }
        }
    }
}

void Freeverb::reset()
{
    for (auto& comb : combs)
        comb.reset();
    
    for (auto& filter : dampingFilters)
        filter.reset();
    
    for (auto& allpass : allpasses)
        allpass.reset();
}

ReverbProcessorParameters& Freeverb::getParameters() { return parameters; }

void Freeverb::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == parameters))
    {
        parameters = params;
        parameters.roomSize = scale(parameters.roomSize, 0.0f, 1.0f, 0.25f, 1.75f);
    }
}

////==============================================================================
//void Freeverb::prepareToPlay(double sampleRate, int samplesPerBlock)
//{
//	juce::dsp::ProcessSpec spec;
//	spec.sampleRate = sampleRate;
//	spec.maximumBlockSize = samplesPerBlock;
//	spec.numChannels = getMainBusNumInputChannels();
//	
////	earlyReflections.prepareToPlay(sampleRate, samplesPerBlock);
//	
//	comb0.prepare(spec);
//	comb1.prepare(spec);
//	comb2.prepare(spec);
//	comb3.prepare(spec);
//	comb4.prepare(spec);
//	comb5.prepare(spec);
//	comb6.prepare(spec);
//	comb7.prepare(spec);
//	
//	dampingFilter0.prepare(spec);
//	dampingFilter0.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	dampingFilter1.prepare(spec);
//	dampingFilter1.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	dampingFilter2.prepare(spec);
//	dampingFilter2.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	dampingFilter3.prepare(spec);
//	dampingFilter3.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	dampingFilter4.prepare(spec);
//	dampingFilter4.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	dampingFilter5.prepare(spec);
//	dampingFilter5.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	dampingFilter6.prepare(spec);
//	dampingFilter6.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	dampingFilter7.prepare(spec);
//	dampingFilter7.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
//	
//	allpass0.prepare(spec);
//	allpass1.prepare(spec);
//	allpass2.prepare(spec);
//	allpass3.prepare(spec);
//	
//	mixer.prepare(spec);
//}

////==============================================================================
//void Freeverb::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
//{
//	juce::ScopedNoDenormals noDenormals;
//	
//	juce::dsp::AudioBlock<float> block { buffer };
//	mixer.setWetMixProportion(mDryWetMix);
//	mixer.pushDrySamples(block);
//	
//	juce::AudioBuffer<float> earlyReflectionsBuffer(buffer.getNumChannels(), buffer.getNumSamples());
//	earlyReflectionsBuffer.clear();
//	earlyReflectionsBuffer.makeCopyOf(buffer, false);
//	
//	// mix between
//	float earlyGain = cos(mEarlyLateMix * M_PI * 0.5);
//	float lateGain = cos((mEarlyLateMix * M_PI * 0.5) + M_PI * 1.5);
//	earlyReflectionsBuffer.applyGain(earlyGain);
//	buffer.applyGain(lateGain);
//	
//	// get early reflection parameters
////	earlyReflections.setSize(mSize);
////	earlyReflections.setDecay(mDecay);
////	
////	earlyReflections.processBlock(earlyReflectionsBuffer, midiMessages);
//	
//	dampingFilter0.setCutoffFrequency(mDampingCutoff);
//	dampingFilter1.setCutoffFrequency(mDampingCutoff);
//	dampingFilter2.setCutoffFrequency(mDampingCutoff);
//	dampingFilter3.setCutoffFrequency(mDampingCutoff);
//	dampingFilter4.setCutoffFrequency(mDampingCutoff);
//	dampingFilter5.setCutoffFrequency(mDampingCutoff);
//	dampingFilter6.setCutoffFrequency(mDampingCutoff);
//	dampingFilter7.setCutoffFrequency(mDampingCutoff);
//	
//	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
//	{
//		auto* channelData = buffer.getWritePointer (channel);
//		
//		comb0.setDelay(1557 * mSize + (channel * stereoWidth));
//		comb1.setDelay(1617 * mSize + (channel * stereoWidth));
//		comb2.setDelay(1491 * mSize + (channel * stereoWidth));
//		comb3.setDelay(1422 * mSize + (channel * stereoWidth));
//		comb4.setDelay(1277 * mSize + (channel * stereoWidth));
//		comb5.setDelay(1356 * mSize + (channel * stereoWidth));
//		comb6.setDelay(1188 * mSize + (channel * stereoWidth));
//		comb7.setDelay(1116 * mSize + (channel * stereoWidth));
//		
//		allpass0.setDelay(225 + (channel * stereoWidth));
//		allpass1.setDelay(556 + (channel * stereoWidth));
//		allpass2.setDelay(441 + (channel * stereoWidth));
//		allpass3.setDelay(341 + (channel * stereoWidth));
//		
//		// comb processing in parallel
//		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
//		{
//			auto currentSample = channelData[sample]/8;
//			
//			comb0Output = comb0.popSample(channel);
//			comb0.pushSample(channel, dampingFilter0.processSample(channel, currentSample + comb0Output * (mDecay)));
//			
//			comb1Output = comb1.popSample(channel);
//			comb1.pushSample(channel, dampingFilter1.processSample(channel, currentSample + comb1Output * (mDecay)));
//			
//			comb2Output = comb2.popSample(channel);
//			comb2.pushSample(channel, dampingFilter2.processSample(channel, currentSample + comb2Output * (mDecay)));
//			
//			comb3Output = comb3.popSample(channel);
//			comb3.pushSample(channel, dampingFilter3.processSample(channel, currentSample + comb3Output * (mDecay)));
//			
//			comb4Output = comb4.popSample(channel);
//			comb4.pushSample(channel, dampingFilter4.processSample(channel, currentSample + comb4Output * (mDecay)));
//			
//			comb5Output = comb5.popSample(channel);
//			comb5.pushSample(channel, dampingFilter5.processSample(channel, currentSample + comb5Output * (mDecay)));
//			
//			comb6Output = comb6.popSample(channel);
//			comb6.pushSample(channel, dampingFilter6.processSample(channel, currentSample + comb6Output * (mDecay)));
//			
//			comb7Output = comb7.popSample(channel);
//			comb7.pushSample(channel, dampingFilter7.processSample(channel, currentSample + comb7Output * (mDecay)));
//			
//			channelData[sample] = comb0Output + comb1Output + comb2Output + comb3Output + comb4Output + comb5Output + comb6Output + comb7Output;
//		}
//		
//		// allpass processing in series
//		float allpassFeedbackCoefficient = 0.5;
//		
//		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
//		{
//			allpass0Output = allpass0.popSample(channel);
//			feedback = allpass0Output * allpassFeedbackCoefficient;
//			feedforward = -channelData[sample] - allpass0Output * allpassFeedbackCoefficient;
//			allpass0.pushSample(channel, channelData[sample] + feedback);
//			channelData[sample] = allpass0Output + feedforward;
//			
//			allpass1Output = allpass1.popSample(channel);
//			feedback = allpass1Output * allpassFeedbackCoefficient;
//			feedforward = -channelData[sample]  - allpass1Output * allpassFeedbackCoefficient;
//			allpass1.pushSample(channel, channelData[sample] + feedback);
//			channelData[sample] = allpass1Output + feedforward;
//			
//			allpass2Output = allpass2.popSample(channel);
//			feedback = allpass2Output * allpassFeedbackCoefficient;
//			feedforward = -channelData[sample] - allpass2Output * allpassFeedbackCoefficient;
//			allpass2.pushSample(channel, channelData[sample] + feedback);
//			channelData[sample] = allpass2Output + feedforward;
//			
//			allpass3Output = allpass3.popSample(channel);
//			feedback = allpass3Output * allpassFeedbackCoefficient;
//			feedforward = -channelData[sample] - allpass2Output * allpassFeedbackCoefficient;
//			allpass3.pushSample(channel, channelData[sample] + feedback);
//			channelData[sample] = allpass3Output + feedforward;
//		}
//	}
//	
//	// mix early/late reflection buffers; add wet samples
//	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
//		buffer.addFrom(channel, 0, earlyReflectionsBuffer, channel, 0, buffer.getNumSamples());
//	juce::dsp::AudioBlock<float> wetBlock { buffer };
//	mixer.mixWetSamples(wetBlock);
//}

////==============================================================================
//
////==============================================================================
//void Freeverb::setSize(float newSize) { mSize = newSize; }
//void Freeverb::setDecay(float newDecay) { mDecay = newDecay; }
//void Freeverb::setDampingCutoff(float newCutoff) { mDampingCutoff = newCutoff; }
//void Freeverb::setPreDelay(float newPreDelay) { mPreDelayTime = newPreDelay; }
//void Freeverb::setEarlyLateMix(float newMix) { mEarlyLateMix = newMix; }
//void Freeverb::setDryWetMix(float newMix) { mDryWetMix = newMix; }
