/*
  ==============================================================================

    Freeverb.cpp
    Created: 3 Jun 2023 4:34:15pm
    Author:  Reilly Spitzfaden

  ==============================================================================
*/

#include "Freeverb.h"

Freeverb::Freeverb() = default;

Freeverb::~Freeverb() = default;

void Freeverb::prepare(const juce::dsp::ProcessSpec& spec)
{
    comb0.prepare(spec);
    comb1.prepare(spec);
    comb2.prepare(spec);
    comb3.prepare(spec);
    comb4.prepare(spec);
    comb5.prepare(spec);
    comb6.prepare(spec);
    comb7.prepare(spec);
    
    dampingFilter0.prepare(spec);
    dampingFilter0.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    dampingFilter1.prepare(spec);
    dampingFilter1.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    dampingFilter2.prepare(spec);
    dampingFilter2.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    dampingFilter3.prepare(spec);
    dampingFilter3.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    dampingFilter4.prepare(spec);
    dampingFilter4.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    dampingFilter5.prepare(spec);
    dampingFilter5.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    dampingFilter6.prepare(spec);
    dampingFilter6.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    dampingFilter7.prepare(spec);
    dampingFilter7.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    
    allpass0.prepare(spec);
    allpass1.prepare(spec);
    allpass2.prepare(spec);
    allpass3.prepare(spec);
    
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

void Freeverb::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    dampingFilter0.setCutoffFrequency(mParameters.damping);
    dampingFilter1.setCutoffFrequency(mParameters.damping);
    dampingFilter2.setCutoffFrequency(mParameters.damping);
    dampingFilter3.setCutoffFrequency(mParameters.damping);
    dampingFilter4.setCutoffFrequency(mParameters.damping);
    dampingFilter5.setCutoffFrequency(mParameters.damping);
    dampingFilter6.setCutoffFrequency(mParameters.damping);
    dampingFilter7.setCutoffFrequency(mParameters.damping);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        lfoParameters = lfo[channel].getParameters();
        lfoParameters.frequency_Hz = mParameters.modRate;
        lfo[channel].setParameters(lfoParameters);
        
        comb0.setDelay(1557 * mParameters.roomSize + (channel * mWidth));
        comb1.setDelay(1617 * mParameters.roomSize + (channel * mWidth));
        comb2.setDelay(1491 * mParameters.roomSize + (channel * mWidth));
        comb3.setDelay(1422 * mParameters.roomSize + (channel * mWidth));
        comb4.setDelay(1277 * mParameters.roomSize + (channel * mWidth));
        comb5.setDelay(1356 * mParameters.roomSize + (channel * mWidth));
        comb6.setDelay(1188 * mParameters.roomSize + (channel * mWidth));
        comb7.setDelay(1116 * mParameters.roomSize + (channel * mWidth));
        
        int allpass0Delay = 225 + (channel * mWidth);
        int allpass2Delay = 441 + (channel * mWidth);
        allpass0.setDelay(allpass0Delay);
        allpass1.setDelay(556 + (channel * mWidth));
        allpass2.setDelay(allpass2Delay);
        allpass3.setDelay(341 + (channel * mWidth));
        
        // comb processing in parallel
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            auto currentSample = channelData[sample]/8;
            
            comb0Output = comb0.popSample(channel);
            comb0.pushSample(channel, dampingFilter0.processSample(channel, currentSample + comb0Output * (mParameters.decayTime)));
            
            comb1Output = comb1.popSample(channel);
            comb1.pushSample(channel, dampingFilter1.processSample(channel, currentSample + comb1Output * (mParameters.decayTime)));
            
            comb2Output = comb2.popSample(channel);
            comb2.pushSample(channel, dampingFilter2.processSample(channel, currentSample + comb2Output * (mParameters.decayTime)));
            
            comb3Output = comb3.popSample(channel);
            comb3.pushSample(channel, dampingFilter3.processSample(channel, currentSample + comb3Output * (mParameters.decayTime)));
            
            comb4Output = comb4.popSample(channel);
            comb4.pushSample(channel, dampingFilter4.processSample(channel, currentSample + comb4Output * (mParameters.decayTime)));
            
            comb5Output = comb5.popSample(channel);
            comb5.pushSample(channel, dampingFilter5.processSample(channel, currentSample + comb5Output * (mParameters.decayTime)));
            
            comb6Output = comb6.popSample(channel);
            comb6.pushSample(channel, dampingFilter6.processSample(channel, currentSample + comb6Output * (mParameters.decayTime)));
            
            comb7Output = comb7.popSample(channel);
            comb7.pushSample(channel, dampingFilter7.processSample(channel, currentSample + comb7Output * (mParameters.decayTime)));
            
            channelData[sample] = comb0Output + comb1Output + comb2Output + comb3Output + comb4Output + comb5Output + comb6Output + comb7Output;
        }
        
        // allpass processing in series
        float allpassFeedbackCoefficient = 0.5;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // LFO
            lfoOutput = lfo[channel].renderAudioOutput();
            
            allpass0Output = allpass0.popSample(channel, allpass0Delay + (lfoOutput.normalOutput * 1.0f * mParameters.modDepth));
            feedback = allpass0Output * allpassFeedbackCoefficient;
            feedforward = -channelData[sample] - allpass0Output * allpassFeedbackCoefficient;
            allpass0.pushSample(channel, channelData[sample] + feedback);
            channelData[sample] = allpass0Output + feedforward;
            
            allpass1Output = allpass1.popSample(channel);
            feedback = allpass1Output * allpassFeedbackCoefficient;
            feedforward = -channelData[sample]  - allpass1Output * allpassFeedbackCoefficient;
            allpass1.pushSample(channel, channelData[sample] + feedback);
            channelData[sample] = allpass1Output + feedforward;
            
            allpass2Output = allpass2.popSample(channel, allpass2Delay + (lfoOutput.quadPhaseOutput_neg * 1.0f * mParameters.modDepth));
            feedback = allpass2Output * allpassFeedbackCoefficient;
            feedforward = -channelData[sample] - allpass2Output * allpassFeedbackCoefficient;
            allpass2.pushSample(channel, channelData[sample] + feedback);
            channelData[sample] = allpass2Output + feedforward;
            
            allpass3Output = allpass3.popSample(channel);
            feedback = allpass3Output * allpassFeedbackCoefficient;
            feedforward = -channelData[sample] - allpass2Output * allpassFeedbackCoefficient;
            allpass3.pushSample(channel, channelData[sample] + feedback);
            channelData[sample] = allpass3Output + feedforward;
        }
    }
}

void Freeverb::reset()
{
    comb0.reset();
    comb1.reset();
    comb2.reset();
    comb3.reset();
    comb4.reset();
    comb5.reset();
    comb6.reset();
    comb7.reset();
    
    dampingFilter0.reset();
    dampingFilter1.reset();
    dampingFilter2.reset();
    dampingFilter3.reset();
    dampingFilter4.reset();
    dampingFilter5.reset();
    dampingFilter6.reset();
    dampingFilter7.reset();
    
    allpass0.reset();
    allpass1.reset();
    allpass2.reset();
    allpass3.reset();
}

ReverbProcessorParameters& Freeverb::getParameters() { return mParameters; }

void Freeverb::setParameters(const ReverbProcessorParameters& params)
{
    if (!(params == mParameters))
        mParameters = params;
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
//		comb0.setDelay(1557 * mSize + (channel * mWidth));
//		comb1.setDelay(1617 * mSize + (channel * mWidth));
//		comb2.setDelay(1491 * mSize + (channel * mWidth));
//		comb3.setDelay(1422 * mSize + (channel * mWidth));
//		comb4.setDelay(1277 * mSize + (channel * mWidth));
//		comb5.setDelay(1356 * mSize + (channel * mWidth));
//		comb6.setDelay(1188 * mSize + (channel * mWidth));
//		comb7.setDelay(1116 * mSize + (channel * mWidth));
//		
//		allpass0.setDelay(225 + (channel * mWidth));
//		allpass1.setDelay(556 + (channel * mWidth));
//		allpass2.setDelay(441 + (channel * mWidth));
//		allpass3.setDelay(341 + (channel * mWidth));
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
