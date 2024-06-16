/*
  ==============================================================================

    LookAndFeel class definition
 
  ==============================================================================
*/

#pragma once

//look and feel of sliders/labels
class GrayBlueLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GrayBlueLookAndFeel()
    {
        // colors
        // sliders
        setColour(juce::Slider::thumbColourId, juce::Colours::aliceblue);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightsteelblue);
        setColour(juce::Slider::trackColourId, juce::Colours::lightslategrey);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::aliceblue);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::aliceblue);
        // labels
        setColour(juce::Label::textColourId, juce::Colours::aliceblue);
        // menus
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xAA88CC)); // doesn't read as actual colour?
        setColour(juce::ComboBox::buttonColourId, juce::Colours::slategrey);
        setColour(juce::ComboBox::textColourId, juce::Colours::aliceblue);
        setColour(juce::ComboBox::outlineColourId, juce::Colours::aliceblue);
        setColour(juce::ComboBox::arrowColourId, juce::Colours::aliceblue);
        
        // fonts
//        setDefaultSansSerifTypeface(juce::LookAndFeel::getTypefaceForFont(juce::Font("Open Sans", 18.0f, juce::Font::plain)));
    }
    
//    juce::Font getLabelFont(juce::Label& label) override
//    {
//        return juce::Font("Open Sans", 18.0f, juce::Font::plain);
//    }
};
