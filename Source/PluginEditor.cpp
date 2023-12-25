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
	using namespace juce;
	
    g.fillAll (Colours::black);
	
	auto bounds = getLocalBounds();
	auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
	auto width = responseArea.getWidth();
	
	auto &lowcut = monoChain.get<ChannelPositions::LowCut>();
	auto &peak = monoChain.get<ChannelPositions::Peak>();
	auto &highcut = monoChain.get<ChannelPositions::HighCut>();
	
	auto sampleRate = audioProcessor.getSampleRate();
	
	std::vector<double> magnitutes;
	magnitutes.resize(width);
	
	
	for (int i = 0; i < width; i++)
	{
		double magnitute = 1.f;
		auto freq = mapToLog10(double(i) / double(width), 20.0, 20000.0);
		
		if (!monoChain.isBypassed<ChannelPositions::Peak>())
		{
			magnitute *= peak.coefficients -> getMagnitudeForFrequency(freq, sampleRate);
		}
		
		if (!lowcut.isBypassed<0>())
		{
			lowcut.get<0>().coefficients -> getMagnitudeForFrequency(freq, sampleRate);
		}
		
		if (!lowcut.isBypassed<1>())
		{
			lowcut.get<1>().coefficients -> getMagnitudeForFrequency(freq, sampleRate);
		}
		
		if (!lowcut.isBypassed<2>())
		{
			lowcut.get<2>().coefficients -> getMagnitudeForFrequency(freq, sampleRate);
		}
		
		if (!lowcut.isBypassed<3>())
		{
			lowcut.get<3>().coefficients -> getMagnitudeForFrequency(freq, sampleRate);
		}
			
		magnitutes[i] = Decibels::gainToDecibels(magnitute);
	}
	
	Path responseCurve;
	
	const double outputMin = responseArea.getBottom();
	const double outputMax = responseArea.getY();
	
	auto map = [outputMin, outputMax](double input)
	{
		return jmap(input, -24.0, 24.0, outputMin,	outputMax);
	};
	
	responseCurve.startNewSubPath(responseArea.getX(), map(magnitutes.front()));
	
	for(size_t i = 0; i < magnitutes.size(); i++)
	{
		responseCurve.lineTo(responseArea.getX() + i,map (magnitutes[i]));
	}
	
	g.setColour(Colours::orange);
	g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.f);
	g.setColour(Colours::white);
	g.strokePath(responseCurve, PathStrokeType(2.f));
	
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

void AwesomeEQAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
	parametersChanged.set(true);
}

void AwesomeEQAudioProcessorEditor::timerCallback()
{
	if(parametersChanged.compareAndSetBool(false, true))
	{
		// update monoChain
		// signal repaint
	}
	
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
