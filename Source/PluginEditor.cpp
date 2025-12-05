// Graphical interface for the plugin

#include "PluginEditor.h"
#include "PluginProcessor.h"

RSAlgorithmicVerbAudioProcessorEditor::RSAlgorithmicVerbAudioProcessorEditor(RSAlgorithmicVerbAudioProcessor& p,
                                                                             juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts)
{
    // labels
    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    roomSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(roomSizeLabel);

    preDelayLabel.setText("Pre-Delay", juce::dontSendNotification);
    preDelayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(preDelayLabel);

    decayLabel.setText("Decay Time", juce::dontSendNotification);
    decayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(decayLabel);

    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dampingLabel);

    diffusionLabel.setText("Diffusion", juce::dontSendNotification);
    diffusionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(diffusionLabel);

    earlySizeLabel.setText("Early Size", juce::dontSendNotification);
    earlySizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(earlySizeLabel);

    earlyDecayLabel.setText("Early Decay", juce::dontSendNotification);
    earlyDecayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(earlyDecayLabel);

    modRateLabel.setText("Mod Rate", juce::dontSendNotification);
    modRateLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modRateLabel);

    modDepthLabel.setText("Mod Depth", juce::dontSendNotification);
    modDepthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modDepthLabel);

    highCutLabel.setText("High Cut", juce::dontSendNotification);
    highCutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(highCutLabel);

    lowCutLabel.setText("Low Cut", juce::dontSendNotification);
    lowCutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lowCutLabel);

    earlyLateMixLabel.setText("Early Reflections Mix", juce::dontSendNotification);
    earlyLateMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(earlyLateMixLabel);

    dryWetMixLabel.setText("Dry/Wet Mix", juce::dontSendNotification);
    dryWetMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryWetMixLabel);

    reverbMenuLabel.setText("Reverb Type:", juce::dontSendNotification);
    reverbMenuLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbMenuLabel);

    // menus
    addAndMakeVisible(reverbMenuBox);
    reverbMenuBox.addSectionHeading("Allpass Rings");
    reverbMenuBox.addItem("Dattorro Plate", dattorro);
    reverbMenuBox.addItem("Dattorro Hall", largeConcertHallB);
    reverbMenuBox.addItem("Gardner Small Room", gardnerSmallRoom);
    reverbMenuBox.addItem("Gardner Medium Room", gardnerMediumRoom);
    reverbMenuBox.addItem("Gardner Large Room", gardnerLargeRoom);
    reverbMenuBox.addSeparator();
    reverbMenuBox.addSectionHeading("Schroeder Reverbs");
    reverbMenuBox.addItem("Freeverb", freeverb);
    reverbMenuBox.addSeparator();
    reverbMenuBox.addSectionHeading("Feedback Delay Networks");
    reverbMenuBox.addItem("Anderson 8th Order FDN", anderson8xFDN);
    reverbMenuBox.addItem("Circulant 8th Order FDN", circulant8xFDN);
    reverbMenuBox.addItem("Hadamard 8th Order FDN", hadamard8xFDN);
    reverbMenuBox.addItem("Householder 8th Order FDN", householder8xFDN);
    reverbMenuBox.addSeparator();
    reverbMenuBox.addSectionHeading("Special FX");
    reverbMenuBox.addItem("Constellation", constellation);
    reverbMenuBox.addItem("Event Horizon", eventHorizon);
    reverbMenuBox.setSelectedId(dattorro);
    reverbMenuBox.setJustificationType(juce::Justification::centred);
    reverbMenuAttachment.reset(new ComboBoxAttachment(valueTreeState, "reverbType", reverbMenuBox));

    // sliders row 1
    roomSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(roomSizeSlider);
    roomSizeAttachment.reset(new SliderAttachment(valueTreeState, "roomSize", roomSizeSlider));

    preDelaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    preDelaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(preDelaySlider);
    preDelayAttachment.reset(new SliderAttachment(valueTreeState, "preDelay", preDelaySlider));

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

    earlySizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    earlySizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(earlySizeSlider);
    earlySizeAttachment.reset(new SliderAttachment(valueTreeState, "earlySize", earlySizeSlider));

    earlyDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    earlyDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(earlyDepthSlider);
    earlyDecayAttachment.reset(new SliderAttachment(valueTreeState, "earlyDecay", earlyDepthSlider));

    modRateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    modRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(modRateSlider);
    modRateAttachment.reset(new SliderAttachment(valueTreeState, "modRate", modRateSlider));

    modDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    modDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(modDepthSlider);
    modDepthAttachment.reset(new SliderAttachment(valueTreeState, "modDepth", modDepthSlider));

    highCutSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    highCutSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(highCutSlider);
    highCutAttachment.reset(new SliderAttachment(valueTreeState, "highCut", highCutSlider));

    lowCutSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lowCutSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible(lowCutSlider);
    lowCutAttachment.reset(new SliderAttachment(valueTreeState, "lowCut", lowCutSlider));

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
    setSize(1350, 600);
}

RSAlgorithmicVerbAudioProcessorEditor::~RSAlgorithmicVerbAudioProcessorEditor()
{
}

//==============================================================================
void RSAlgorithmicVerbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(32, 32, 32));

    // Title
    g.setColour(juce::Colours::aliceblue);
    g.setFont(32.0f);
    g.drawFittedText("RS Algorithmic Verb", 25, 10, 350, 45, juce::Justification::left, 1);
    // Info
    g.setFont(16.0f);
    g.drawFittedText("Version 0.5.5\n reillyspitzfaden.com", getWidth() - 375, 15, 350, 45, juce::Justification::right,
                     2);

    // panels
    //    g.setColour(juce::Colour::fromRGB(68, 81, 96));
    g.setColour(juce::Colour::fromRGB(95, 68, 88));
    g.fillRoundedRectangle(25, 80, getWidth() - 50, getHeight() - 160, 25);
}

void RSAlgorithmicVerbAudioProcessorEditor::resized()
{
    const int xBorder = 30;
    const int yBorderTop = 100;
    const int yBorderBottom = 65;
    const int rowSpacer = 50;
    const int bottomMenuSpacer = 5;

    const int textLabelWidth = 150;
    const int textLabelHeight = 20;
    const int textLabelSpacer = 7;

    const int menuWidth = 225;
    const int menuHeight = 20;
    const int sliderWidth1 = (getWidth() - (2 * xBorder)) / 8;
    const int sliderWidth2 = sliderWidth1 * 2;
    const int sliderHeight1 =
        (getHeight() - yBorderTop - yBorderBottom - rowSpacer - (2 * textLabelHeight) - bottomMenuSpacer - menuHeight) /
        2;
    const int sliderHeight2 = sliderHeight1 * 1.75;
    const int slider2Buffer = 45;

    // Sliders
    roomSizeSlider.setBounds(xBorder, yBorderTop, sliderWidth1, sliderHeight1);
    preDelaySlider.setBounds(xBorder, yBorderTop + sliderHeight1 + rowSpacer, sliderWidth1, sliderHeight1);

    feedbackSlider.setBounds(xBorder + sliderWidth1, yBorderTop + slider2Buffer, sliderWidth2, sliderHeight2);

    dampingSlider.setBounds(xBorder + (3 * sliderWidth1), yBorderTop, sliderWidth1, sliderHeight1);
    diffusionSlider.setBounds(xBorder + (3 * sliderWidth1), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth1,
                              sliderHeight1);

    earlySizeSlider.setBounds(xBorder + (4 * sliderWidth1), yBorderTop, sliderWidth1, sliderHeight1);
    earlyDepthSlider.setBounds(xBorder + (4 * sliderWidth1), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth1,
                               sliderHeight1);

    modRateSlider.setBounds(xBorder + (5 * sliderWidth1), yBorderTop, sliderWidth1, sliderHeight1);
    modDepthSlider.setBounds(xBorder + (5 * sliderWidth1), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth1,
                             sliderHeight1);

    highCutSlider.setBounds(xBorder + (6 * sliderWidth1), yBorderTop, sliderWidth1, sliderHeight1);
    lowCutSlider.setBounds(xBorder + (6 * sliderWidth1), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth1,
                           sliderHeight1);

    earlyLateMixSlider.setBounds(xBorder + (7 * sliderWidth1), yBorderTop, sliderWidth1, sliderHeight1);
    dryWetMixSlider.setBounds(xBorder + (7 * sliderWidth1), yBorderTop + sliderHeight1 + rowSpacer, sliderWidth1,
                              sliderHeight1);

    // Labels
    roomSizeLabel.setBounds(xBorder + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                            yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
    preDelayLabel.setBounds(xBorder + (sliderWidth1 / 2) - (textLabelWidth / 2),
                            yBorderTop + (2 * sliderHeight1) + rowSpacer + textLabelSpacer, textLabelWidth,
                            textLabelHeight);

    decayLabel.setBounds(xBorder + sliderWidth1 + ((sliderWidth2 / 2) - (textLabelWidth / 2)),
                         yBorderTop + sliderHeight2 + slider2Buffer + textLabelSpacer, textLabelWidth, textLabelHeight);

    dampingLabel.setBounds(xBorder + (sliderWidth1 * 3) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                           yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
    diffusionLabel.setBounds(xBorder + (sliderWidth1 * 3) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                             yBorderTop + (2 * sliderHeight1) + rowSpacer + textLabelSpacer, textLabelWidth,
                             textLabelHeight);

    earlySizeLabel.setBounds(xBorder + (sliderWidth1 * 4) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                             yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
    earlyDecayLabel.setBounds(xBorder + (sliderWidth1 * 4) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                              yBorderTop + (2 * sliderHeight1) + rowSpacer + textLabelSpacer, textLabelWidth,
                              textLabelHeight);

    modRateLabel.setBounds(xBorder + (sliderWidth1 * 5) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                           yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
    modDepthLabel.setBounds(xBorder + (sliderWidth1 * 5) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                            yBorderTop + (2 * sliderHeight1) + rowSpacer + textLabelSpacer, textLabelWidth,
                            textLabelHeight);

    highCutLabel.setBounds(xBorder + (sliderWidth1 * 6) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                           yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
    lowCutLabel.setBounds(xBorder + (sliderWidth1 * 6) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                          yBorderTop + (2 * sliderHeight1) + rowSpacer + textLabelSpacer, textLabelWidth,
                          textLabelHeight);

    earlyLateMixLabel.setBounds(xBorder + (sliderWidth1 * 7) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                                yBorderTop + sliderHeight1 + textLabelSpacer, textLabelWidth, textLabelHeight);
    dryWetMixLabel.setBounds(xBorder + (sliderWidth1 * 7) + ((sliderWidth1 / 2) - (textLabelWidth / 2)),
                             yBorderTop + (2 * sliderHeight1) + rowSpacer + textLabelSpacer, textLabelWidth,
                             textLabelHeight);

    // menu
    reverbMenuBox.setBounds(getWidth() - menuWidth - 25, getHeight() - menuHeight - 45, menuWidth, menuHeight);
    reverbMenuBox.setJustificationType(juce::Justification::left);
    reverbMenuLabel.setBounds(getWidth() - menuWidth - textLabelWidth - 25, getHeight() - menuHeight - 45,
                              textLabelWidth, menuHeight);
    reverbMenuLabel.setJustificationType(juce::Justification::right);
}
