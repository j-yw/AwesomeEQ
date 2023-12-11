/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AwesomeEQAudioProcessorEditor::AwesomeEQAudioProcessorEditor (AwesomeEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
peakFreqSliderAttachment(audioProcessor.parameters, "Peak Freq", peakFreqSlider),
peakGainSliderAttachment(audioProcessor.parameters, "Peak Gain", peakGainSlider),
peakQualitySliderAttachment(audioProcessor.parameters, "Peak Quality", peakQualitySlider),

lowCutFreqSliderAttachment(audioProcessor.parameters, "LowCut Freq", lowCutFreqSlider),
lowCutSlopeSliderAttachment(audioProcessor.parameters, "LowCut Slope", lowCutSlopeSlider),

highCutFreqSliderAttachment(audioProcessor.parameters, "HighCut Freq", highCutFreqSlider),
highCutSlopeSliderAttachment(audioProcessor.parameters, "HighCut Slope", highCutSlopeSlider)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	
	for (auto *component : getComps()){
		addAndMakeVisible(component);
	}
	
    setSize (400, 300);
}

AwesomeEQAudioProcessorEditor::~AwesomeEQAudioProcessorEditor()
{
}

//==============================================================================
void AwesomeEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AwesomeEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	auto bounds = getLocalBounds();
	auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
	auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
	auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
	
	lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
	lowCutSlopeSlider.setBounds(lowCutArea);
	highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
	highCutSlopeSlider.setBounds(highCutArea);
	
	peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
	peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
	peakQualitySlider.setBounds(bounds);
}

std::vector<juce::Component*> AwesomeEQAudioProcessorEditor::getComps()
{
	return
	{
		&peakFreqSlider,
		&peakGainSlider,
		&peakQualitySlider,
		&lowCutFreqSlider,
		&lowCutSlopeSlider,
		&highCutFreqSlider,
		&highCutSlopeSlider
	};
}
