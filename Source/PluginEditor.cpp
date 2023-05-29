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
	
	roomSizeLabel.setText("Room Size", juce::dontSendNotification);
	roomSizeLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(roomSizeLabel);
	
	decayLabel.setText("Decay Time", juce::dontSendNotification);
	decayLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(decayLabel);
	
	dampingLabel.setText("Damping", juce::dontSendNotification);
	dampingLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(dampingLabel);
	
	preDelayLabel.setText("Pre-Delay", juce::dontSendNotification);
	preDelayLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(preDelayLabel);
	
	earlyLateMixLabel.setText("Early/Late Reflections", juce::dontSendNotification);
	earlyLateMixLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(earlyLateMixLabel);
	
	dryWetMixLabel.setText("Dry/Wet Mix", juce::dontSendNotification);
	dryWetMixLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(dryWetMixLabel);
	
	// menus
	addAndMakeVisible(reverbMenuBox);
	reverbMenuBox.addSectionHeading("Allpass Rings");
	reverbMenuBox.addItem("Dattorro Plate", dattorro);
	reverbMenuBox.addSeparator();
	//reverbMenuBox.addSectionHeading("Feedback Delay Networks");
	//reverbMenuBox.addSeparator();
	reverbMenuBox.addSectionHeading("Schroeder Reverbs");
	reverbMenuBox.addItem("Freeverb", freeverb);
	reverbMenuBox.setSelectedId(dattorro);
	reverbMenuAttachment.reset(new ComboBoxAttachment(valueTreeState, "reverbType", reverbMenuBox));
	
	// sliders
	roomSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, 25);
	addAndMakeVisible(roomSizeSlider);
	roomSizeAttachment.reset(new SliderAttachment(valueTreeState, "roomSize", roomSizeSlider));
	
	decaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, 25);
	addAndMakeVisible(decaySlider);
	decayAttachment.reset(new SliderAttachment(valueTreeState, "decayTime", decaySlider));
	
	dampingSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	dampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, 25);
	addAndMakeVisible(dampingSlider);
	dampingAttachment.reset(new SliderAttachment(valueTreeState, "damping", dampingSlider));
	
	preDelaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	preDelaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, 25);
	addAndMakeVisible(preDelaySlider);
	preDelayAttachment.reset(new SliderAttachment(valueTreeState, "preDelay", preDelaySlider));
	
	earlyLateMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	earlyLateMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, 25);
	addAndMakeVisible(earlyLateMixSlider);
	earlyLateMixAttachment.reset(new SliderAttachment(valueTreeState, "earlyLateMix", earlyLateMixSlider));
	
	dryWetMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	dryWetMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, 25);
	addAndMakeVisible(dryWetMixSlider);
	decayAttachment.reset(new SliderAttachment(valueTreeState, "dryWetMix", dryWetMixSlider));

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
	g.drawFittedText ("RS Algorithmic Verb", 0, 10, getWidth(), 50, juce::Justification::centred, 1);
}

void RSAlgorithmicVerbAudioProcessorEditor::resized()
{
	const int xBorder = 30;
	const int yBorderTop = 125;
	const int yBorderBottom = 70;
	const int rowSpacer = 35;
	
	const int menuWidth = 135;
	const int menuHeight = 20;
	const int sliderWidth = (getWidth() - (2 * xBorder)) / 3;
	const int sliderHeight = (getHeight() - (yBorderTop + yBorderBottom) - rowSpacer) / 2;
	const int textLabelWidth = 150;
	const int textLabelHeight = 20;
	const int textLabelSpacer = 7;
	
	reverbMenuBox.setBounds((getWidth() / 2) - (menuWidth / 2), 65, menuWidth, menuHeight);
	reverbMenuLabel.setBounds((getWidth() / 2) - (menuWidth / 2), 95, menuWidth, menuHeight);
	reverbMenuLabel.setJustificationType(juce::Justification::centred);
	
	// row 1 sliders
	roomSizeSlider.setBounds(xBorder, yBorderTop, sliderWidth, sliderHeight);
	decaySlider.setBounds(xBorder + sliderWidth, yBorderTop, sliderWidth, sliderHeight);
	dampingSlider.setBounds(xBorder + (2 * sliderWidth), yBorderTop, sliderWidth, sliderHeight);
	
	// row 1 labels
	roomSizeLabel.setBounds(xBorder + ((sliderWidth / 2) - (textLabelWidth / 2)), yBorderTop + sliderHeight + textLabelSpacer, textLabelWidth, textLabelHeight);
	decayLabel.setBounds(xBorder + sliderWidth + ((sliderWidth / 2) - (textLabelWidth / 2)), yBorderTop + sliderHeight + textLabelSpacer, textLabelWidth, textLabelHeight);
	dampingLabel.setBounds(xBorder + (sliderWidth * 2) + ((sliderWidth / 2) - (textLabelWidth / 2)), yBorderTop + sliderHeight + textLabelSpacer, textLabelWidth, textLabelHeight);
	
	// row 2 sliders
	preDelaySlider.setBounds(xBorder, yBorderTop + sliderHeight + rowSpacer, sliderWidth, sliderHeight);
	earlyLateMixSlider.setBounds(xBorder + sliderWidth, yBorderTop + sliderHeight + rowSpacer, sliderWidth, sliderHeight);
	dryWetMixSlider.setBounds(xBorder + (2 * sliderWidth), yBorderTop + sliderHeight + rowSpacer, sliderWidth, sliderHeight);
	
	// row 2 labels
	preDelayLabel.setBounds(xBorder + (sliderWidth / 2) - (textLabelWidth / 2), yBorderTop + (sliderHeight * 2) + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
	earlyLateMixLabel.setBounds(xBorder + sliderWidth + (sliderWidth / 2) - (textLabelWidth / 2), yBorderTop + (sliderHeight * 2) + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
	dryWetMixLabel.setBounds(xBorder + (sliderWidth * 2) + (sliderWidth / 2) - (textLabelWidth / 2), yBorderTop + (sliderHeight * 2) + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
}
