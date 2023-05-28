/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//look and feel of sliders/labels
class GrayBlueLookAndFeel : public juce::LookAndFeel_V4
{
public:
	GrayBlueLookAndFeel()
	{
		// colors
		// menus
		setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xAA88CC)); // doesn't read as actual colour?
		setColour(juce::ComboBox::buttonColourId, juce::Colours::slategrey);
		setColour(juce::ComboBox::textColourId, juce::Colours::aliceblue);
		setColour(juce::ComboBox::outlineColourId, juce::Colours::aliceblue);
		setColour(juce::ComboBox::arrowColourId, juce::Colours::aliceblue);
		// sliders
		setColour(juce::Slider::thumbColourId, juce::Colours::aliceblue);
		setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightsteelblue);
		setColour(juce::Slider::trackColourId, juce::Colours::slategrey);
		setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::aliceblue);
		setColour(juce::Slider::textBoxTextColourId, juce::Colours::aliceblue);
		// labels
		setColour(juce::Label::textColourId, juce::Colours::aliceblue);
	}
};

//==============================================================================
/**
*/
class RSAlgorithmicVerbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RSAlgorithmicVerbAudioProcessorEditor (RSAlgorithmicVerbAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~RSAlgorithmicVerbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
	
	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RSAlgorithmicVerbAudioProcessor& audioProcessor;
	
	juce::AudioProcessorValueTreeState& valueTreeState;
	
	juce::Label reverbMenuLabel;
	juce::Label roomSizeLabel;
	juce::Label feedbackLabel;
	juce::Label dampingLabel;
	juce::Label mixLabel;
	juce::Label widthLabel;
	
	juce::ComboBox reverbMenuBox;
	enum reverbTypes
	{
		dattorro = 1,
		freeverb,
	};
	
	juce::Slider roomSizeSlider;
	juce::Slider feedbackSlider;
	juce::Slider dampingSlider;
	juce::Slider mixSlider;
	juce::Slider widthSlider;
	
	std::unique_ptr<ComboBoxAttachment> reverbMenuAttachment;
	std::unique_ptr<SliderAttachment> roomSizeAttachment;
	std::unique_ptr<SliderAttachment> feedbackAttachment;
	std::unique_ptr<SliderAttachment> dampingAttachment;
	std::unique_ptr<SliderAttachment> mixAttachment;
	std::unique_ptr<SliderAttachment> widthAttachment;
	
	GrayBlueLookAndFeel grayBlueLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RSAlgorithmicVerbAudioProcessorEditor)
};
