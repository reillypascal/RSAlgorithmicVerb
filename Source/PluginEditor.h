/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiStyles.h"
#include "PluginProcessor.h"

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
	
	// Labels
	juce::Label roomSizeLabel;
    juce::Label preDelayLabel;
    
	juce::Label decayLabel;
    
	juce::Label dampingLabel;
	juce::Label diffusionLabel;
    
    juce::Label earlySizeLabel;
    juce::Label earlyDecayLabel;
    
    juce::Label modRateLabel;
    juce::Label modDepthLabel;
    
    juce::Label highCutLabel;
    juce::Label lowCutLabel;
    
	juce::Label earlyLateMixLabel;
	juce::Label dryWetMixLabel;
	
    juce::Label reverbMenuLabel;
    
	// Sliders
	juce::Slider roomSizeSlider;
    juce::Slider preDelaySlider;
    
	juce::Slider feedbackSlider;
    
	juce::Slider dampingSlider;
	juce::Slider diffusionSlider;
    
    juce::Slider earlySizeSlider;
    juce::Slider earlyDepthSlider;
    
    juce::Slider modRateSlider;
    juce::Slider modDepthSlider;
	
    juce::Slider highCutSlider;
	juce::Slider lowCutSlider;
    
	juce::Slider earlyLateMixSlider;
	juce::Slider dryWetMixSlider;
    
    juce::ComboBox reverbMenuBox;
    enum reverbTypes
    {
        dattorro = 1,
        largeConcertHallB,
        gardnerSmallRoom,
        gardnerMediumRoom,
        gardnerLargeRoom,
        freeverb,
        stautnerPuckette82FDN
    };
    
	// attachments
	std::unique_ptr<SliderAttachment> roomSizeAttachment;
    std::unique_ptr<SliderAttachment> preDelayAttachment;
    
	std::unique_ptr<SliderAttachment> feedbackAttachment;
    
	std::unique_ptr<SliderAttachment> dampingAttachment;
	std::unique_ptr<SliderAttachment> diffusionAttachment;
    
    std::unique_ptr<SliderAttachment> earlySizeAttachment;
    std::unique_ptr<SliderAttachment> earlyDecayAttachment;
    
    std::unique_ptr<SliderAttachment> modRateAttachment;
    std::unique_ptr<SliderAttachment> modDepthAttachment;
    
    std::unique_ptr<SliderAttachment> highCutAttachment;
	std::unique_ptr<SliderAttachment> lowCutAttachment;
    
	std::unique_ptr<SliderAttachment> earlyLateMixAttachment;
	std::unique_ptr<SliderAttachment> dryWetMixAttachment;
    
    std::unique_ptr<ComboBoxAttachment> reverbMenuAttachment;
	
	const int textBoxWidth = 70;
	const int textBoxHeight = 25;
	GrayBlueLookAndFeel grayBlueLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RSAlgorithmicVerbAudioProcessorEditor)
};
