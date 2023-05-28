/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RSAlgorithmicVerbAudioProcessorEditor::RSAlgorithmicVerbAudioProcessorEditor (RSAlgorithmicVerbAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
	// labels
	reverbMenuLabel.setText("Reverb Type", juce::dontSendNotification);
	reverbMenuLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(reverbMenuLabel);
	
	// menus
	addAndMakeVisible(reverbMenuBox);
	reverbMenuBox.addSectionHeading("Allpass Rings");
	reverbMenuBox.addItem("Dattorro Plate", dattorro);
	reverbMenuBox.addSeparator();
	reverbMenuBox.addSectionHeading("Feedback Delay Networks");
	reverbMenuBox.addSeparator();
	reverbMenuBox.addSectionHeading("Schroeder Reverbs");
	reverbMenuBox.addItem("Freeverb", freeverb);
	//reverbMenuBox.setSelectedId(dattorro);
	reverbMenuAttachment.reset(new ComboBoxAttachment(valueTreeState, "reverbType", reverbMenuBox));
	
	// sliders
	
	// interface style
	getLookAndFeel().setDefaultLookAndFeel(&grayBlueLookAndFeel);
	
	// panel size
	setSize (700, 575);
}

RSAlgorithmicVerbAudioProcessorEditor::~RSAlgorithmicVerbAudioProcessorEditor()
{
}

//==============================================================================
void RSAlgorithmicVerbAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll (juce::Colour(0x221144));

	g.setColour (juce::Colours::aliceblue);
	g.setFont (32.0f);
	g.drawFittedText ("RSAlgorithmicVerb", 0, 10, getWidth(), 50, juce::Justification::centred, 1);
}

void RSAlgorithmicVerbAudioProcessorEditor::resized()
{
	//int xBorder = 30;
	//int yBorder = 70;
	//int rowSpacer = 10;
	
	reverbMenuLabel.setBounds((getWidth() / 2) - 75, 95, 150, 20);
	reverbMenuLabel.setJustificationType(juce::Justification::centred);
	reverbMenuBox.setBounds((getWidth() / 2) - 75, 65, 150, 20);
}
