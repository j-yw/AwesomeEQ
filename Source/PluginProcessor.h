/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope
{
	Slope_12,
	Slope_24,
	Slope_36,
	Slope_48
};

struct ChannelSettings
{
	float peakFreq { 0 }, peakGainInDecibels { 0 }, peakQuality {1.f};
	float lowCutFreq { 0 }, highCutFreq { 0 };
	Slope lowCutSlope { Slope::Slope_12 }, highCutSlope { Slope::Slope_12 };
};

ChannelSettings getChannelSettings(juce::AudioProcessorValueTreeState& parameters);
//==============================================================================
/**
*/

using Filter = juce::dsp::IIR::Filter<float>;
using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
	
enum ChannelPositions
{
	LowCut,
	Peak,
	HighCut
};

using Coefficients = Filter::CoefficientsPtr;
void updateCoefficients(Coefficients& old, const Coefficients& replacements);

Coefficients makePeakFilter(const ChannelSettings& channelSettings, double sampleRate);

class AwesomeEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AwesomeEQAudioProcessor();
    ~AwesomeEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
	
	// Parameter Connection to DSP
	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	juce::AudioProcessorValueTreeState parameters {*this, nullptr, "Parameters", createParameterLayout()};

private:
	MonoChain leftChannel, rightChannel;
	
	void updatePeakFilter(const ChannelSettings& ChannelSettings);
	
	template<int Index, typename ChannelType, typename CoefficientType>
	void update (ChannelType& channel, const CoefficientType& cutCoefficients)
	{
		updateCoefficients(channel.template get<Index>().coefficients, cutCoefficients[Index]);
		channel.template setBypassed<Index>(false);
	}

	template<typename ChannelType, typename CoefficientType>
	void updateCutFilter(ChannelType& channel, const CoefficientType& cutCoefficients, const Slope& lowCutSlope)
	{
		channel.template setBypassed<0>(true);
		channel.template setBypassed<1>(true);
		channel.template setBypassed<2>(true);
		channel.template setBypassed<3>(true);
		
		switch (lowCutSlope)
		{
			case Slope_48:
			{
				update<3>(channel, cutCoefficients);
			}
			case Slope_36:
			{
				update<2>(channel, cutCoefficients);
			}
			case Slope_24:
			{
				update<1>(channel, cutCoefficients);
			}
			case Slope_12:
			{
				update<0>(channel, cutCoefficients);
			}
		}
	}
	
	void updateLowCutFilters(const ChannelSettings& channelSettings);
	void updateHighCutFilters(const ChannelSettings& channelSettings);
	void updateFilters();
	
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AwesomeEQAudioProcessor)
};
