/*
  ==============================================================================

TODO:
 - louder wet sound on rooms
 - EQ to wet only
 - Barr allpass ring
 - early reflections
	- add parallel/series at 0.33/0.67 mark
	- note that series early -> late slows onset/suggests larger hall!
 - "Bloom" effect & more moderate version for room/chamber
	- early -> late series
	- nested allpasses - echoes build up when recirculated
 - disable appropriate parameters when changing algorithms
 - fix decay parameter name
 
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RSAlgorithmicVerbAudioProcessor::RSAlgorithmicVerbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
//	mainProcessor(new juce::AudioProcessorGraph()),
	parameters(*this, nullptr, juce::Identifier("RSAlgorithmicVerb"), {
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "roomSize", 1 },
													"Room Size",
													0.0f,
													1.0f,
													0.5f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "preDelay", 1 },
                                                    "Pre-Delay",
                                                    juce::NormalisableRange<float>(0.0f,
                                                                                   150.0f,
                                                                                   1.0f),
                                                    0.0f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "feedback", 1 },
													"Feedback",
													0.0f,
													0.99f,
													0.35f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "damping", 1 },
													"Damping",
													0.0f,
													1.0f,
													0.5f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "diffusion", 1 },
													"Diffusion",
													0.0f,
													0.99f,
													0.67f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "earlySize", 1 },
                                                    "Early Size",
                                                    0.0f,
                                                    1.0f,
                                                    0.5f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "earlyDecay", 1 },
                                                    "Early Decay",
                                                    0.0f,
                                                    0.99f,
                                                    0.35f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "modRate", 1 },
                                                    "Mod Rate",
                                                    0.0f,
                                                    5.0f,
                                                    0.25f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "modDepth", 1 },
                                                    "Mod Depth",
                                                    0.0f,
                                                    1.0f,
                                                    0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "highCut", 1 },
                                                    "High Cut",
                                                    juce::NormalisableRange<float>(200,
                                                                                 20000,
                                                                                 5),
                                                    20000),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "lowCut", 1 },
													"Low Cut",
													juce::NormalisableRange<float>(20,
																				 1000,
																				 5),
													20),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "earlyLateMix", 1 },
													"Early Reflections Mix",
													0.0f,
													1.0f,
													0.0f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "dryWetMix", 1 },
													"Dry/Wet Mix",
													0.0f,
													1.0f,
													0.35f),
        std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "reverbType", 1 },
                                                     "Reverb Type",
                                                     juce::StringArray { "DattorroPlate", "DattorroHall", "SmallRoom", "MediumRoom", "LargeRoom", "Freeverb", "Anderson8xFDN", "circulant8xFDN", "Hadamard8xFDN", "Householder8xFDN", "Constellation" },
                                                     0)
})
{ 
    
}

RSAlgorithmicVerbAudioProcessor::~RSAlgorithmicVerbAudioProcessor()
{ 
    
}

//==============================================================================
const juce::String RSAlgorithmicVerbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RSAlgorithmicVerbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RSAlgorithmicVerbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RSAlgorithmicVerbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RSAlgorithmicVerbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RSAlgorithmicVerbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RSAlgorithmicVerbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RSAlgorithmicVerbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RSAlgorithmicVerbAudioProcessor::getProgramName (int index)
{
    return {};
}

void RSAlgorithmicVerbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RSAlgorithmicVerbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getMainBusNumInputChannels();
    
    // pre-delay
    preDelay.prepare(spec);
    preDelay.setMaximumDelayInSamples((sampleRate / 4) + samplesPerBlock);
    // low-cut
    lowCutFilter.prepare(spec);
    lowCutFilter.reset();
    *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f);
    //high-cut
    highCutFilter.prepare(spec);
    highCutFilter.reset();
    *highCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 20000.0f);
    // early reflections
    earlyReflections.prepare(spec);
	// mixers
    earlyLevelMixer.prepare(spec);
    earlyLevelMixer.reset();
	dryWetMixer.prepare(spec);
	dryWetMixer.reset();
}

void RSAlgorithmicVerbAudioProcessor::releaseResources()
{
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RSAlgorithmicVerbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RSAlgorithmicVerbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //============ housekeeping ============
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
//    auto numChannels = buffer.getNumChannels();
//    auto numSamples = buffer.getNumSamples();
    
    //================ mixer settings + dry ================
    // settings to mix btwn early/dry into reverb processor
    earlyLevelMixer.setWetMixProportion(parameters.getRawParameterValue("earlyLateMix")->load());
    // dry/wet mixer settings
    dryWetMixer.setWetMixProportion(parameters.getRawParameterValue("dryWetMix")->load());
    
    juce::dsp::AudioBlock<float> dryBlock { buffer };
    earlyLevelMixer.pushDrySamples(dryBlock);
    dryWetMixer.pushDrySamples(dryBlock);
    
    //================ filters, pre-delay ================
    // context
    juce::dsp::AudioBlock<float> preBlock { buffer };
    
    // filters
    *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), parameters.getRawParameterValue("lowCut")->load());
    *highCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), parameters.getRawParameterValue("highCut")->load());
    lowCutFilter.process(juce::dsp::ProcessContextReplacing<float>(preBlock));
    highCutFilter.process(juce::dsp::ProcessContextReplacing<float>(preBlock));
    
    // pre-delay
    preDelay.setDelay(parameters.getRawParameterValue("preDelay")->load() * (getSampleRate() / 1000));
    preDelay.process(juce::dsp::ProcessContextReplacing<float>(preBlock));
    
    //================ early reflections processor ================
    // early reflections parameters
    earlyParameters = earlyReflections.getParameters();
    earlyParameters.decayTime = parameters.getRawParameterValue("earlyDecay")->load();
    earlyParameters.roomSize = parameters.getRawParameterValue("earlySize")->load();
    earlyReflections.setParameters(earlyParameters);
    
    // early reflections mono/stereo - prevents comb filtering on reverbs that mix input to mono
    std::vector<bool> earlyMonoFlagsPerProcessor {true, false, false, false, false, true, false};
    earlyReflections.setMonoFlag(earlyMonoFlagsPerProcessor[static_cast<juce::AudioParameterChoice*>(parameters.getParameter("reverbType"))->getIndex()]);
    
    //================ process early reflections ================
    earlyReflections.processBlock(buffer, midiMessages);
    juce::dsp::AudioBlock<float> earlyBlock { buffer };
    earlyLevelMixer.mixWetSamples(earlyBlock);
    
    //=============== reverb processor ================
    slotProcessor = static_cast<juce::AudioParameterChoice*>(parameters.getParameter("reverbType"))->getIndex();
    
    //============ update processor ============
    if (slotProcessor != prevSlotProcessor)
    {
        reverbProcessor = processorFactory.create(slotProcessor);
        
        if (reverbProcessor != nullptr)
        {
            juce::dsp::ProcessSpec spec;
            spec.sampleRate = getSampleRate();
            spec.maximumBlockSize = buffer.getNumSamples();
            spec.numChannels = buffer.getNumChannels();
            
            reverbProcessor->prepare(spec);
        }
        
        prevSlotProcessor = slotProcessor;
    }
    
    //============ run processor ============
    if (reverbProcessor != nullptr)
    {
        //============ get parameters ============
        reverbParameters = reverbProcessor->getParameters();
        
        //============ new parameters in class ============
        reverbParameters.damping = scale(parameters.getRawParameterValue("damping")->load() * -1.0f + 1.0f, 0.0f, 1.0f, 200.0f, 20000.0f);
        reverbParameters.decayTime = parameters.getRawParameterValue("feedback")->load();
        reverbParameters.diffusion = parameters.getRawParameterValue("diffusion")->load();
        reverbParameters.modDepth = parameters.getRawParameterValue("modDepth")->load();
        reverbParameters.modRate = parameters.getRawParameterValue("modRate")->load();
        reverbParameters.roomSize = scale(parameters.getRawParameterValue("roomSize")->load(), 0.0f, 1.0f, 0.25f, 1.75f);
        
        //============ set parameters ============
        reverbProcessor->setParameters(reverbParameters);
        
        //============ process reverb ============
        reverbProcessor->processBlock(buffer, midiMessages);
    }
    
    //============ mix in reverb wet ============
    juce::dsp::AudioBlock<float> wetBlock { buffer };
    dryWetMixer.mixWetSamples(wetBlock);
    
//	updateGraph();
	
	// get processors to set parameters
//	ProcessorBase* currentProcessorNode = static_cast<ProcessorBase*>(reverbNode->getProcessor());
	
	// reverb node parameters
//	currentProcessorNode->setSize(size);
//	currentProcessorNode->setDecay(feedback);
//	currentProcessorNode->setDampingCutoff(damping);
//	currentProcessorNode->setDiffusion(diffusion);
//	currentProcessorNode->setPreDelay(preDelay);
//	currentProcessorNode->setEarlyLateMix(earlyLateMix);
//	currentProcessorNode->setDryWetMix(dryWetMix);
	
	// processing
//	mainProcessor->processBlock(buffer, midiMessages);
}

//==============================================================================
bool RSAlgorithmicVerbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RSAlgorithmicVerbAudioProcessor::createEditor()
{
    return new RSAlgorithmicVerbAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void RSAlgorithmicVerbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml (state.createXml());
	copyXmlToBinary(*xml, destData);
}

void RSAlgorithmicVerbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
	
	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RSAlgorithmicVerbAudioProcessor();
}

//void RSAlgorithmicVerbAudioProcessor::initialiseGraph()
//{
//	mainProcessor->clear();
//	
//	audioInputNode  = mainProcessor->addNode
//		(std::make_unique<AudioGraphIOProcessor>
//		(AudioGraphIOProcessor::audioInputNode));
//	audioOutputNode = mainProcessor->addNode
//		(std::make_unique<AudioGraphIOProcessor>
//		(AudioGraphIOProcessor::audioOutputNode));
//	midiInputNode   = mainProcessor->addNode
//		(std::make_unique<AudioGraphIOProcessor>
//		(AudioGraphIOProcessor::midiInputNode));
//	midiOutputNode  = mainProcessor->addNode
//		(std::make_unique<AudioGraphIOProcessor>
//		(AudioGraphIOProcessor::midiOutputNode));
//	
//	connectAudioNodes();
//	connectMidiNodes();
//}

//void RSAlgorithmicVerbAudioProcessor::updateGraph()
//{
//	bool hasChanged = false;
//	
//	// responds to dropdown
//	switch(reverbType->getIndex())
//	{
//		case 0:
//			if (reverbNode != nullptr)
//			{
//				if (reverbNode->getProcessor()->getName() == "DattorroPlate")
//					break;
//				
//				reverbNode->getProcessor()->reset();
//				mainProcessor->removeNode(reverbNode.get());
//			}
//			
//			reverbNode = mainProcessor->addNode(std::make_unique<DattorroPlate>());
//			hasChanged = true;
//			break;
//			
//		case 1:
//			if (reverbNode != nullptr)
//			{
//				if (reverbNode->getProcessor()->getName() == "LargeConcertHallB")
//					break;
//				
//				reverbNode->getProcessor()->reset();
//				mainProcessor->removeNode(reverbNode.get());
//			}
//			
//			reverbNode = mainProcessor->addNode(std::make_unique<LargeConcertHallB>());
//			hasChanged = true;
//			break;
//			
//		case 2:
//			if (reverbNode != nullptr)
//			{
//				if (reverbNode->getProcessor()->getName() == "GardnerSmallRoom")
//					break;
//				
//				reverbNode->getProcessor()->reset();
//				mainProcessor->removeNode(reverbNode.get());
//			}
//			
//			reverbNode = mainProcessor->addNode(std::make_unique<GardnerSmallRoom>());
//			hasChanged = true;
//			break;
//			
//		case 3:
//			if (reverbNode != nullptr)
//			{
//				if (reverbNode->getProcessor()->getName() == "GardnerMediumRoom")
//					break;
//				
//				reverbNode->getProcessor()->reset();
//				mainProcessor->removeNode(reverbNode.get());
//			}
//			
//			reverbNode = mainProcessor->addNode(std::make_unique<GardnerMediumRoom>());
//			hasChanged = true;
//			break;
//			
//		case 4:
//			if (reverbNode != nullptr)
//			{
//				if (reverbNode->getProcessor()->getName() == "GardnerLargeRoom")
//					break;
//				
//				reverbNode->getProcessor()->reset();
//				mainProcessor->removeNode(reverbNode.get());
//			}
//			
//			reverbNode = mainProcessor->addNode(std::make_unique<GardnerLargeRoom>());
//			hasChanged = true;
//			break;
//			
//		case 5:
//			if (reverbNode != nullptr)
//			{
//				if (reverbNode->getProcessor()->getName() == "Freeverb")
//					break;
//				
//				reverbNode->getProcessor()->reset();
//				mainProcessor->removeNode(reverbNode.get());
//			}
//			
//			reverbNode = mainProcessor->addNode(std::make_unique<Freeverb>());
//			hasChanged = true;
//			break;
//			
//		default:
//			if (reverbNode != nullptr)
//			{
//				reverbNode->getProcessor()->reset();
//				mainProcessor->removeNode(reverbNode.get());
//				reverbNode = nullptr;
//				hasChanged = true;
//			}
//			
//			break;
//	}
//	
//	if (hasChanged)
//	{
//		for (auto connection : mainProcessor->getConnections())
//			mainProcessor->removeConnection(connection);
//				
//		if (reverbNode == nullptr)
//		{
//			connectAudioNodes();
//			connectMidiNodes();
//		}
//		else
//		{
//			for (int channel = 0; channel < getMainBusNumInputChannels(); ++channel)
//			{
//				mainProcessor->addConnection({ { audioInputNode->nodeID, channel },
//											   { reverbNode->nodeID, channel } });
//				mainProcessor->addConnection({ { reverbNode->nodeID, channel },
//											   { audioOutputNode->nodeID, channel } });
//			}
//		}
//		
//		mainProcessor->addConnection ({ { midiInputNode->nodeID,
//										  juce::AudioProcessorGraph::midiChannelIndex },
//										{ reverbNode->nodeID,
//										  juce::AudioProcessorGraph::midiChannelIndex } });
//		mainProcessor->addConnection ({ { reverbNode->nodeID,
//										  juce::AudioProcessorGraph::midiChannelIndex },
//										{ midiOutputNode->nodeID,
//										  juce::AudioProcessorGraph::midiChannelIndex } });
//		
//		//connectMidiNodes();
//		for (auto node : mainProcessor->getNodes())
//			node->getProcessor()->enableAllBuses();
//	}
//}

//void RSAlgorithmicVerbAudioProcessor::connectAudioNodes()
//{
//   for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
//	   mainProcessor->addConnection ({ { audioInputNode->nodeID, channel },
//									   { audioOutputNode->nodeID, channel } });
//}

//void RSAlgorithmicVerbAudioProcessor::connectMidiNodes()
//{
//   mainProcessor->addConnection ({ { midiInputNode->nodeID,
//	   juce::AudioProcessorGraph::midiChannelIndex },
//	   { midiOutputNode->nodeID,
//		   juce::AudioProcessorGraph::midiChannelIndex
//	   } });
//}

//float RSAlgorithmicVerbAudioProcessor::scale(float input, float inLow, float inHi, float outLow, float outHi)
//{
//	float scaleFactor = (outHi - outLow)/(inHi - inLow);
//	float offset = outLow - inLow;
//	return (input * scaleFactor) + offset;
//}
