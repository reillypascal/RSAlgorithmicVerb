// Audio processor for the plugin

#pragma once

#include <JuceHeader.h>

#include "ConcertHallB.h"
// #include "CustomDelays.h"
#include "DattorroVerb.h"
#include "EarlyReflections.h"
#include "FDNs.h"
#include "Freeverb.h"
#include "GardnerRooms.h"
#include "ProcessorBase.h"
#include "SpecialFX.h"
#include "Utilities.h"

struct ProcessorFactory
{
    std::unique_ptr<ReverbProcessorBase> create(int type)
    {
        auto iter = processorMapping.find(type);
        if (iter != processorMapping.end())
            return iter->second();
        
        return nullptr;
    }
    
    std::map<int,
             std::function<std::unique_ptr<ReverbProcessorBase>()>> processorMapping
    {
        { 0, []() { return std::make_unique<DattorroPlate>(); } },
        { 1, []() { return std::make_unique<LargeConcertHallB>(); } },
        { 2, []() { return std::make_unique<GardnerSmallRoom>(); } },
        { 3, []() { return std::make_unique<GardnerMediumRoom>(); } },
        { 4, []() { return std::make_unique<GardnerLargeRoom>(); } },
        { 5, []() { return std::make_unique<Freeverb>(); } },
        { 6, []() { return std::make_unique<GeneralizedFDN>(8, "Anderson"); } },
        { 7, []() { return std::make_unique<GeneralizedFDN>(8, "circulant"); } },
        { 8, []() { return std::make_unique<GeneralizedFDN>(8, "Hadamard"); } },
        { 9, []() { return std::make_unique<GeneralizedFDN>(8, "Householder"); } },
        { 10, []() { return std::make_unique<Constellation>(); } },
        { 11, []() { return std::make_unique<EventHorizon>(); } }
    };
};

class RSAlgorithmicVerbAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
	using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
	using Node = juce::AudioProcessorGraph::Node;
	
    //==============================================================================
    RSAlgorithmicVerbAudioProcessor();
    ~RSAlgorithmicVerbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
//	void initialiseGraph();
//	
//	void updateGraph();
//	
//	void connectAudioNodes();
//	
//	void connectMidiNodes();
	
//	float scale(float input, float inLow, float inHi, float outLow, float outHi);
			
//	std::unique_ptr<juce::AudioProcessorGraph> mainProcessor = nullptr;
	
//	Node::Ptr audioInputNode = nullptr;
//	Node::Ptr audioOutputNode = nullptr;
//	Node::Ptr midiInputNode = nullptr;
//	Node::Ptr midiOutputNode = nullptr;
//	
//	Node::Ptr reverbNode = nullptr;
	
	juce::AudioProcessorValueTreeState parameters;
    
    juce::dsp::DelayLine<float> preDelay { 22050 };
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowCutFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highCutFilter;
    
    EarlyReflections earlyReflections;
    ReverbProcessorParameters earlyParameters;
    
    ProcessorFactory processorFactory {};
    std::unique_ptr<ReverbProcessorBase> reverbProcessor = std::unique_ptr<ReverbProcessorBase> {};
    ReverbProcessorParameters reverbParameters;
    
    juce::dsp::DryWetMixer<float> earlyLevelMixer;
    juce::dsp::DryWetMixer<float> dryWetMixer;
    
    int slotProcessor { -1 };
    int prevSlotProcessor { -1 };
	
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RSAlgorithmicVerbAudioProcessor)
};
