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
	// labels row 1
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
	
	diffusionLabel.setText("Diffusion", juce::dontSendNotification);
	diffusionLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(diffusionLabel);
	
	// labels row 2
	lowCutLabel.setText("Low Cut", juce::dontSendNotification);
	lowCutLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(lowCutLabel);
	
	highCutLabel.setText("High Cut", juce::dontSendNotification);
	highCutLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(highCutLabel);
	
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
	reverbMenuBox.addItem("Gardner Small Room", gardnerSmallRoom);
	reverbMenuBox.addSeparator();
	//reverbMenuBox.addSectionHeading("Feedback Delay Networks");
	//reverbMenuBox.addSeparator();
	reverbMenuBox.addSectionHeading("Schroeder Reverbs");
	reverbMenuBox.addItem("Freeverb", freeverb);
	reverbMenuBox.setSelectedId(dattorro);
	reverbMenuBox.setJustificationType(juce::Justification::centred);
	reverbMenuAttachment.reset(new ComboBoxAttachment(valueTreeState, "reverbType", reverbMenuBox));
	
	// sliders row 1
	roomSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(roomSizeSlider);
	roomSizeAttachment.reset(new SliderAttachment(valueTreeState, "roomSize", roomSizeSlider));
	
	feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(feedbackSlider);
	feedbackAttachment.reset(new SliderAttachment(valueTreeState, "feedback", feedbackSlider));
	
	dampingSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	dampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(dampingSlider);
	dampingAttachment.reset(new SliderAttachment(valueTreeState, "damping", dampingSlider));
	
	diffusionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	diffusionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(diffusionSlider);
	diffusionAttachment.reset(new SliderAttachment(valueTreeState, "diffusion", diffusionSlider));
	
	// sliders row 2
	lowCutSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	lowCutSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(lowCutSlider);
	lowCutAttachment.reset(new SliderAttachment(valueTreeState, "lowCut", lowCutSlider));
	
	highCutSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	highCutSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(highCutSlider);
	highCutAttachment.reset(new SliderAttachment(valueTreeState, "highCut", highCutSlider));
	
	preDelaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	preDelaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(preDelaySlider);
	preDelayAttachment.reset(new SliderAttachment(valueTreeState, "preDelay", preDelaySlider));
	
	earlyLateMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	earlyLateMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(earlyLateMixSlider);
	earlyLateMixAttachment.reset(new SliderAttachment(valueTreeState, "earlyLateMix", earlyLateMixSlider));
	
	dryWetMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	dryWetMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
	addAndMakeVisible(dryWetMixSlider);
	dryWetMixAttachment.reset(new SliderAttachment(valueTreeState, "dryWetMix", dryWetMixSlider));

	// interface style
	getLookAndFeel().setDefaultLookAndFeel(&grayBlueLookAndFeel);
	
	// panel size
	setSize (800, 525);
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
	const int yBorderTop = 135;
	const int yBorderBottom = 50;
	const int rowSpacer = 45;
	
	const int menuWidth = 200;
	const int menuHeight = 20;
	const int sliderWidth1 = (getWidth() - (2 * xBorder)) / 4;
	const int sliderWidth2 = (getWidth() - (2 * xBorder)) / 5;
	const int sliderHeight1 = (getHeight() - (yBorderTop + yBorderBottom) - rowSpacer) / 2;
	const int sliderHeight2 = sliderHeight1 * 0.8;
	const int textLabelWidth = 150;
	const int textLabelHeight = 20;
	const int textLabelSpacer = 7;
	
	reverbMenuBox.setBounds((getWidth() / 2) - (menuWidth / 2), 65, menuWidth, menuHeight);
	reverbMenuLabel.setBounds((getWidth() / 2) - (menuWidth / 2), 95, menuWidth, menuHeight);
	reverbMenuLabel.setJustificationType(juce::Justification::centred);
	
	// row 1 sliders
	roomSizeSlider.setBounds(xBorder, yBorderTop, sliderWidth1, sliderHeight1);
	feedbackSlider.setBounds(xBorder + sliderWidth1, yBorderTop, sliderWidth1, sliderHeight1);
	dampingSlider.setBounds(xBorder + (2 * sliderWidth1), yBorderTop, sliderWidth1, sliderHeight1);
	diffusionSlider.setBounds(xBorder + (3 * sliderWidth1), yBorderTop, sliderWidth1, sliderHeight1);
	
	// row 1 labels
	roomSizeLabel.setBounds(xBorder + ((sliderWidth1 / 2) - (textLabelWidth / 2)), yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
	decayLabel.setBounds(xBorder + sliderWidth1 + ((sliderWidth1 / 2) - (textLabelWidth / 2)), yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
	dampingLabel.setBounds(xBorder + (sliderWidth1 * 2) + ((sliderWidth1 / 2) - (textLabelWidth / 2)), yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
	diffusionLabel.setBounds(xBorder + (sliderWidth1 * 3) + ((sliderWidth1 / 2) - (textLabelWidth / 2)), yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
	
	
	// row 2 sliders
	preDelaySlider.setBounds(xBorder, yBorderTop + sliderHeight1 + rowSpacer, sliderWidth2, sliderHeight2);
	lowCutSlider.setBounds(xBorder + (sliderWidth2 * 1), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth2, sliderHeight2);
	highCutSlider.setBounds(xBorder + (sliderWidth2 * 2), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth2, sliderHeight2);
	earlyLateMixSlider.setBounds(xBorder + (sliderWidth2 * 3), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth2, sliderHeight2);
	dryWetMixSlider.setBounds(xBorder + (sliderWidth2 * 4), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth2, sliderHeight2);
	
	// row 2 labels
	preDelayLabel.setBounds(xBorder + (sliderWidth2 / 2) - (textLabelWidth / 2), yBorderTop + sliderHeight1 + sliderHeight2 + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
	lowCutLabel.setBounds(xBorder + (sliderWidth2 * 1) + (sliderWidth2 / 2) - (textLabelWidth / 2), yBorderTop + sliderHeight1 + sliderHeight2 + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
	highCutLabel.setBounds(xBorder + (sliderWidth2 * 2) + (sliderWidth2 / 2) - (textLabelWidth / 2), yBorderTop + sliderHeight1 + sliderHeight2 + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
	earlyLateMixLabel.setBounds(xBorder + (sliderWidth2 * 3) + (sliderWidth2 / 2) - (textLabelWidth / 2), yBorderTop + sliderHeight1 + sliderHeight2 + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
	dryWetMixLabel.setBounds(xBorder + (sliderWidth2 * 4) + (sliderWidth2 / 2) - (textLabelWidth / 2), yBorderTop + sliderHeight1 + sliderHeight2 + rowSpacer + textLabelSpacer, textLabelWidth, textLabelHeight);
}
