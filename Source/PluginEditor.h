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
	// row 1
	juce::Label roomSizeLabel;
	juce::Label decayLabel;
	juce::Label dampingLabel;
	juce::Label diffusionLabel;
	// row 2
	juce::Label preDelayLabel;
	juce::Label lowCutLabel;
	juce::Label highCutLabel;
	juce::Label earlyLateMixLabel;
	juce::Label dryWetMixLabel;
	
	juce::ComboBox reverbMenuBox;
	enum reverbTypes
	{
		dattorro = 1,
		largeConcertHallB,
		gardnerSmallRoom,
		gardnerMediumRoom,
		gardnerLargeRoom,
		freeverb,
	};
	// row 1
	juce::Slider roomSizeSlider;
	juce::Slider feedbackSlider;
	juce::Slider dampingSlider;
	juce::Slider diffusionSlider;
	// row 2
	juce::Slider preDelaySlider;
	juce::Slider lowCutSlider;
	juce::Slider highCutSlider;
	juce::Slider earlyLateMixSlider;
	juce::Slider dryWetMixSlider;
	
	std::unique_ptr<ComboBoxAttachment> reverbMenuAttachment;
	// row 1
	std::unique_ptr<SliderAttachment> roomSizeAttachment;
	std::unique_ptr<SliderAttachment> feedbackAttachment;
	std::unique_ptr<SliderAttachment> dampingAttachment;
	std::unique_ptr<SliderAttachment> diffusionAttachment;
	// row 2
	std::unique_ptr<SliderAttachment> preDelayAttachment;
	std::unique_ptr<SliderAttachment> lowCutAttachment;
	std::unique_ptr<SliderAttachment> highCutAttachment;
	std::unique_ptr<SliderAttachment> earlyLateMixAttachment;
	std::unique_ptr<SliderAttachment> dryWetMixAttachment;
	
	const int textBoxWidth = 70;
	const int textBoxHeight = 25;
	GrayBlueLookAndFeel grayBlueLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RSAlgorithmicVerbAudioProcessorEditor)
};
