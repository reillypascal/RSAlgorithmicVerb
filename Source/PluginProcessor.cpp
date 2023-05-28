/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

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
	parameters(*this, nullptr, juce::Identifier("RSRetroVerbTest1"), {
		std::make_unique<juce::AudioParameterChoice>("reverbType",
													 "Reverb Type",
													 juce::StringArray { "Dattorro", "Freeverb" },
													 0),
		std::make_unique<juce::AudioParameterFloat>("roomSize",
													"Room Size",
													0.0f,
													1.0f,
													0.5f),
		std::make_unique<juce::AudioParameterFloat>("decay",
													"Decay",
													0.0f,
													0.99f,
													0.35f),
		std::make_unique<juce::AudioParameterFloat>("damping",
													"Damping",
													0.0f,
													1.0f,
													0.75f),
		std::make_unique<juce::AudioParameterFloat>("preDelay",
													"Pre-Delay",
													0.0f,
													1.0f,
													0.5f),
		std::make_unique<juce::AudioParameterFloat>("earlyLateMix",
													"Early/Late Mix",
													0.0f,
													1.0f,
													1.0f),
		std::make_unique<juce::AudioParameterFloat>("dryWetMix",
													"Dry/Wet Mix",
													0.0f,
													1.0f,
													0.1f)
})
{
	reverbType = static_cast<juce::AudioParameterChoice*>(parameters.getParameter("reverbType"));
	
	roomSizeParameter = parameters.getRawParameterValue("roomSize");
	decayParameter = parameters.getRawParameterValue("decay");
	dampingParameter = parameters.getRawParameterValue("damping");
	preDelayParameter = parameters.getRawParameterValue("preDelay");
	earlyLateMixParameter = parameters.getRawParameterValue("earlyLateMix");
	dryWetMixParameter = parameters.getRawParameterValue("dryWetMix");
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
	
	// parameter values
	float size = scale(static_cast<float>(*roomSizeParameter), 0.0f, 1.0f, 0.01f, 2.0f);
	float decay = static_cast<float>(*decayParameter);
	float damping = scale(static_cast<float>(*dampingParameter) * -1 + 1, 0.0f, 1.0f, 200.0f, 20000.0f);
	float preDelay = scale(static_cast<float>(*preDelayParameter), 0.0f, 1.0f, 0.0f, 125.0f);
	float earlyLateMix = static_cast<float>(*earlyLateMixParameter);
	float dryWetMix = static_cast<float>(*dryWetMixParameter);
	
	updateGraph();
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
				
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<DattorroPlate>());
			hasChanged = true;
			
			break;
			
		case 1:
			if (reverbNode != nullptr)
			{
				if (reverbNode->getProcessor()->getName() == "Freeverb")
					break;
				
				mainProcessor->removeNode(reverbNode.get());
			}
			
			reverbNode = mainProcessor->addNode(std::make_unique<Freeverb>());
			hasChanged = true;
			break;
			
		default:
			if (reverbNode != nullptr)
			{
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
		
		connectMidiNodes();
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
