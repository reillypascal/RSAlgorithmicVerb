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
	mainProcessor(new juce::AudioProcessorGraph()),
	parameters(*this, nullptr, juce::Identifier("RSAlgorithmicVerb"), {
		std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "reverbType", 1 },
													 "Reverb Type",
													 juce::StringArray { "Dattorro", "SmallRoom", "MediumRoom", "LargeRoom", "Freeverb" },
													 0),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "roomSize", 1 },
													"Room Size",
													0.0f,
													1.0f,
													0.5f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "feedback", 1 },
													"Feedback",
													0.0f,
													0.99f,
													0.35f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "damping", 1 },
													"Damping",
													0.0f,
													1.0f,
													0.75f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "diffusion", 1 },
													"Diffusion",
													0.0f,
													0.99f,
													0.75f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "preDelay", 1 },
													"Pre-Delay",
													0.0f,
													1.0f,
													0.0f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "lowCut", 1 },
													"Low Cut",
													juce::NormalisableRange<float>(0,
																				 1000,
																				 5),
													0),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "highCut", 1 },
													"High Cut",
													juce::NormalisableRange<float>(200,
																				 20000,
																				 5),
													20000),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "earlyLateMix", 1 },
													"Early/Late Mix",
													0.0f,
													1.0f,
													1.0f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "dryWetMix", 1 },
													"Dry/Wet Mix",
													0.0f,
													1.0f,
													0.35f)
})
{
	reverbType = static_cast<juce::AudioParameterChoice*>(parameters.getParameter("reverbType"));
	// row 1
	roomSizeParameter = parameters.getRawParameterValue("roomSize");
	feedbackParameter = parameters.getRawParameterValue("feedback");
	dampingParameter = parameters.getRawParameterValue("damping");
	diffusionParameter = parameters.getRawParameterValue("diffusion");
	// row 2
	preDelayParameter = parameters.getRawParameterValue("preDelay");
	lowCutParameter = parameters.getRawParameterValue("lowCut");
	highCutParameter = parameters.getRawParameterValue("highCut");
	earlyLateMixParameter = parameters.getRawParameterValue("earlyLateMix");
	dryWetMixParameter = parameters.getRawParameterValue("dryWetMix");
	
}

RSAlgorithmicVerbAudioProcessor::~RSAlgorithmicVerbAudioProcessor()
{
    //mainProcessor->clear();
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
	mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
										getMainBusNumOutputChannels(),
										sampleRate, samplesPerBlock);
	
	mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);
	
	initialiseGraph();
	
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getMainBusNumInputChannels();
	
	earlyLateMixer.prepare(spec);
	earlyLateMixer.reset();
	dryWetMixer.prepare(spec);
	dryWetMixer.reset();
}

void RSAlgorithmicVerbAudioProcessor::releaseResources()
{
	mainProcessor->releaseResources();
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
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
	
	// parameter values row 1
	float size = scale(roomSizeParameter->load(), 0.0f, 1.0f, 0.25f, 1.75f);
	float feedback = feedbackParameter->load();
	float damping = scale(dampingParameter->load() * -1 + 1, 0.0f, 1.0f, 200.0f, 20000.0f);
	float diffusion = diffusionParameter->load();
	// row 2
	float preDelay = scale(preDelayParameter->load(), 0.0f, 1.0f, 0.0f, 250.0f);
	//float lowCut = lowCutParameter->load();
	//float highCut = highCutParameter->load();
	float earlyLateMix = earlyLateMixParameter->load();
	float dryWetMix = dryWetMixParameter->load();
		
	updateGraph();
	
	// get processors to set parameters
	ProcessorBase* currentProcessorNode = static_cast<ProcessorBase*>(reverbNode->getProcessor());
	
	// reverb node parameters
	currentProcessorNode->setSize(size);
	currentProcessorNode->setDecay(feedback);
	currentProcessorNode->setDampingCutoff(damping);
	currentProcessorNode->setDiffusion(diffusion);
	currentProcessorNode->setPreDelay(preDelay);
	currentProcessorNode->setEarlyLateMix(earlyLateMix);
	currentProcessorNode->setDryWetMix(dryWetMix);
	
	// processing
	mainProcessor->processBlock(buffer, midiMessages);
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

void RSAlgorithmicVerbAudioProcessor::initialiseGraph()
{
	mainProcessor->clear();
	
	audioInputNode  = mainProcessor->addNode
		(std::make_unique<AudioGraphIOProcessor>
		(AudioGraphIOProcessor::audioInputNode));
	audioOutputNode = mainProcessor->addNode
		(std::make_unique<AudioGraphIOProcessor>
		(AudioGraphIOProcessor::audioOutputNode));
	midiInputNode   = mainProcessor->addNode
		(std::make_unique<AudioGraphIOProcessor>
		(AudioGraphIOProcessor::midiInputNode));
	midiOutputNode  = mainProcessor->addNode
		(std::make_unique<AudioGraphIOProcessor>
		(AudioGraphIOProcessor::midiOutputNode));
	
	connectAudioNodes();
	connectMidiNodes();
}

void RSAlgorithmicVerbAudioProcessor::updateGraph()
{
	bool hasChanged = false;
	
	// responds to dropdown
	switch(reverbType->getIndex())
	{
		case 0:
			if (reverbNode != nullptr)
			{
				if (reverbNode->getProcessor()->getName() == "DattorroPlate")
					break;
				
				reverbNode->getProcessor()->reset();
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<DattorroPlate>());
			hasChanged = true;
			break;
			
		case 1:
			if (reverbNode != nullptr)
			{
				if (reverbNode->getProcessor()->getName() == "LargeConcertHallB")
					break;
				
				reverbNode->getProcessor()->reset();
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<LargeConcertHallB>());
			hasChanged = true;
			break;
			
		case 2:
			if (reverbNode != nullptr)
			{
				if (reverbNode->getProcessor()->getName() == "GardnerSmallRoom")
					break;
				
				reverbNode->getProcessor()->reset();
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<GardnerSmallRoom>());
			hasChanged = true;
			break;
			
		case 3:
			if (reverbNode != nullptr)
			{
				if (reverbNode->getProcessor()->getName() == "GardnerMediumRoom")
					break;
				
				reverbNode->getProcessor()->reset();
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<GardnerMediumRoom>());
			hasChanged = true;
			break;
			
		case 4:
			if (reverbNode != nullptr)
			{
				if (reverbNode->getProcessor()->getName() == "GardnerLargeRoom")
					break;
				
				reverbNode->getProcessor()->reset();
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<GardnerLargeRoom>());
			hasChanged = true;
			break;
			
		case 5:
			if (reverbNode != nullptr)
			{
				if (reverbNode->getProcessor()->getName() == "Freeverb")
					break;
				
				reverbNode->getProcessor()->reset();
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<Freeverb>());
			hasChanged = true;
			break;
			
		default:
			if (reverbNode != nullptr)
			{
				reverbNode->getProcessor()->reset();
				mainProcessor->removeNode(reverbNode.get());
				reverbNode = nullptr;
				hasChanged = true;
			}
			
			break;
	}
	
	if (hasChanged)
	{
		for (auto connection : mainProcessor->getConnections())
			mainProcessor->removeConnection(connection);
				
		if (reverbNode == nullptr)
		{
			connectAudioNodes();
			connectMidiNodes();
		}
		else
		{
			for (int channel = 0; channel < getMainBusNumInputChannels(); ++channel)
			{
				mainProcessor->addConnection({ { audioInputNode->nodeID, channel },
											   { reverbNode->nodeID, channel } });
				mainProcessor->addConnection({ { reverbNode->nodeID, channel },
											   { audioOutputNode->nodeID, channel } });
			}
		}
		
		mainProcessor->addConnection ({ { midiInputNode->nodeID,
										  juce::AudioProcessorGraph::midiChannelIndex },
										{ reverbNode->nodeID,
										  juce::AudioProcessorGraph::midiChannelIndex } });
		mainProcessor->addConnection ({ { reverbNode->nodeID,
										  juce::AudioProcessorGraph::midiChannelIndex },
										{ midiOutputNode->nodeID,
										  juce::AudioProcessorGraph::midiChannelIndex } });
		
		//connectMidiNodes();
		for (auto node : mainProcessor->getNodes())
			node->getProcessor()->enableAllBuses();
	}
}

void RSAlgorithmicVerbAudioProcessor::connectAudioNodes()
{
   for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
	   mainProcessor->addConnection ({ { audioInputNode->nodeID, channel },
									   { audioOutputNode->nodeID, channel } });
}

void RSAlgorithmicVerbAudioProcessor::connectMidiNodes()
{
   mainProcessor->addConnection ({ { midiInputNode->nodeID,
	   juce::AudioProcessorGraph::midiChannelIndex },
	   { midiOutputNode->nodeID,
		   juce::AudioProcessorGraph::midiChannelIndex
	   } });
}

float RSAlgorithmicVerbAudioProcessor::scale(float input, float inLow, float inHi, float outLow, float outHi)
{
	float scaleFactor = (outHi - outLow)/(inHi - inLow);
	float offset = outLow - inLow;
	return (input * scaleFactor) + offset;
}
