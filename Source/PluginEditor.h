/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider{
	CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
	{
		//
	}
	};

//==============================================================================
/**
*/
class AwesomeEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AwesomeEQAudioProcessorEditor (AwesomeEQAudioProcessor&);
    ~AwesomeEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AwesomeEQAudioProcessor& audioProcessor;
	
	CustomRotarySlider 
	peakFreqSlider,
	peakGainSlider,
	peakQualitySlider, 
	lowCutFreqSlider,
	lowCutSlopeSlider,
	highCutFreqSlider,
	highCutSlopeSlider;
	
	using PARAMETERS = juce::AudioProcessorValueTreeState;
	using Attachment = PARAMETERS::SliderAttachment;
	
	Attachment
	peakFreqSliderAttachment,
	peakGainSliderAttachment,
	peakQualitySliderAttachment,
	lowCutFreqSliderAttachment,
	lowCutSlopeSliderAttachment,
	highCutFreqSliderAttachment,
	highCutSlopeSliderAttachment;
	
	std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AwesomeEQAudioProcessorEditor)
};
