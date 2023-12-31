/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AwesomeEQAudioProcessor::AwesomeEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

AwesomeEQAudioProcessor::~AwesomeEQAudioProcessor()
{
}

//==============================================================================
const juce::String AwesomeEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AwesomeEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AwesomeEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AwesomeEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AwesomeEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AwesomeEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AwesomeEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AwesomeEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AwesomeEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void AwesomeEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AwesomeEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	juce::dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 1;
	spec.sampleRate = sampleRate;
	
	leftChannel.prepare(spec);
	rightChannel.prepare(spec);
	
	auto channelSettings = getChannelSettings(parameters);
	auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, channelSettings.peakFreq, channelSettings.peakQuality, juce::Decibels::decibelsToGain(channelSettings.peakGainInDecibels));
	
	*leftChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
	*rightChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
	
	auto cutCoefficient = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(channelSettings.lowCutFreq,sampleRate, (channelSettings.lowCutSlope + 1) * 2);
	
	auto &leftLowcut = leftChannel.get<ChainPositions::Lowcut>();
	leftLowcut.setBypassed<0>(true);
	leftLowcut.setBypassed<1>(true);
	leftLowcut.setBypassed<2>(true);
	leftLowcut.setBypassed<3>(true);
	
	switch (channelSettings.lowCutSlope)
	{
		case Slope_12:
		{
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			break;
		}
			
		case Slope_24:
		{
			
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			*leftLowcut.get<1>().coefficients = *cutCoefficient[1];
			leftLowcut.setBypassed<1>(false);
			break;
		}
			
		case Slope_36:
		{
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			*leftLowcut.get<1>().coefficients = *cutCoefficient[1];
			leftLowcut.setBypassed<1>(false);
			*leftLowcut.get<2>().coefficients = *cutCoefficient[2];
			leftLowcut.setBypassed<2>(false);
			break;
		}
			
		case Slope_48:
		{
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			*leftLowcut.get<1>().coefficients = *cutCoefficient[1];
			leftLowcut.setBypassed<1>(false);
			*leftLowcut.get<2>().coefficients = *cutCoefficient[2];
			leftLowcut.setBypassed<2>(false);
			*leftLowcut.get<3>().coefficients = *cutCoefficient[3];
			leftLowcut.setBypassed<3>(false);
			break;
		}
	}
	
	
	
	
	auto &rightLowcut = rightChannel.get<ChainPositions::Lowcut>();
	rightLowcut.setBypassed<0>(true);
	rightLowcut.setBypassed<1>(true);
	rightLowcut.setBypassed<2>(true);
	rightLowcut.setBypassed<3>(true);
	
	switch (channelSettings.lowCutSlope)
	{
		case Slope_12:
		{
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			break;
		}
			
		case Slope_24:
		{
			
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			*rightLowcut.get<1>().coefficients = *cutCoefficient[1];
			rightLowcut.setBypassed<1>(false);
			break;
		}
			
		case Slope_36:
		{
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			*rightLowcut.get<1>().coefficients = *cutCoefficient[1];
			rightLowcut.setBypassed<1>(false);
			*rightLowcut.get<2>().coefficients = *cutCoefficient[2];
			rightLowcut.setBypassed<2>(false);
			break;
		}
			
		case Slope_48:
		{
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			*rightLowcut.get<1>().coefficients = *cutCoefficient[1];
			rightLowcut.setBypassed<1>(false);
			*rightLowcut.get<2>().coefficients = *cutCoefficient[2];
			rightLowcut.setBypassed<2>(false);
			*rightLowcut.get<3>().coefficients = *cutCoefficient[3];
			rightLowcut.setBypassed<3>(false);
			break;
		}
	}
}

void AwesomeEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AwesomeEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AwesomeEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
	
	auto channelSettings = getChannelSettings(parameters);
	auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), channelSettings.peakFreq, channelSettings.peakQuality, juce::Decibels::decibelsToGain(channelSettings.peakGainInDecibels));
	
	
	auto cutCoefficient = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(channelSettings.lowCutFreq, getSampleRate(), (channelSettings.lowCutSlope + 1) * 2);
	
	auto &leftLowcut = leftChannel.get<ChainPositions::Lowcut>();
	leftLowcut.setBypassed<0>(true);
	leftLowcut.setBypassed<1>(true);
	leftLowcut.setBypassed<2>(true);
	leftLowcut.setBypassed<3>(true);
	
	switch (channelSettings.lowCutSlope)
	{
		case Slope_12:
		{
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			break;
		}
			
		case Slope_24:
		{
			
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			*leftLowcut.get<1>().coefficients = *cutCoefficient[1];
			leftLowcut.setBypassed<1>(false);
			break;
		}
			
		case Slope_36:
		{
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			*leftLowcut.get<1>().coefficients = *cutCoefficient[1];
			leftLowcut.setBypassed<1>(false);
			*leftLowcut.get<2>().coefficients = *cutCoefficient[2];
			leftLowcut.setBypassed<2>(false);
			break;
		}
			
		case Slope_48:
		{
			*leftLowcut.get<0>().coefficients = *cutCoefficient[0];
			leftLowcut.setBypassed<0>(false);
			*leftLowcut.get<1>().coefficients = *cutCoefficient[1];
			leftLowcut.setBypassed<1>(false);
			*leftLowcut.get<2>().coefficients = *cutCoefficient[2];
			leftLowcut.setBypassed<2>(false);
			*leftLowcut.get<3>().coefficients = *cutCoefficient[3];
			leftLowcut.setBypassed<3>(false);
			break;
		}
	}
	
	
	
	
	auto &rightLowcut = rightChannel.get<ChainPositions::Lowcut>();
	rightLowcut.setBypassed<0>(true);
	rightLowcut.setBypassed<1>(true);
	rightLowcut.setBypassed<2>(true);
	rightLowcut.setBypassed<3>(true);
	
	switch (channelSettings.lowCutSlope)
	{
		case Slope_12:
		{
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			break;
		}
			
		case Slope_24:
		{
			
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			*rightLowcut.get<1>().coefficients = *cutCoefficient[1];
			rightLowcut.setBypassed<1>(false);
			break;
		}
			
		case Slope_36:
		{
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			*rightLowcut.get<1>().coefficients = *cutCoefficient[1];
			rightLowcut.setBypassed<1>(false);
			*rightLowcut.get<2>().coefficients = *cutCoefficient[2];
			rightLowcut.setBypassed<2>(false);
			break;
		}
			
		case Slope_48:
		{
			*rightLowcut.get<0>().coefficients = *cutCoefficient[0];
			rightLowcut.setBypassed<0>(false);
			*rightLowcut.get<1>().coefficients = *cutCoefficient[1];
			rightLowcut.setBypassed<1>(false);
			*rightLowcut.get<2>().coefficients = *cutCoefficient[2];
			rightLowcut.setBypassed<2>(false);
			*rightLowcut.get<3>().coefficients = *cutCoefficient[3];
			rightLowcut.setBypassed<3>(false);
			break;
		}
	}
	
	*leftChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
	*rightChannel.get<ChainPositions::Peak>().coefficients = *peakCoefficients;

	juce::dsp::AudioBlock<float> block(buffer);
	auto leftBlock = block.getSingleChannelBlock(0);
	auto rightBlock = block.getSingleChannelBlock(1);
	juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
	juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
	leftChannel.process(leftContext);
	rightChannel.process(rightContext);
	
	
	
}

//==============================================================================
bool AwesomeEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AwesomeEQAudioProcessor::createEditor()
{
	return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AwesomeEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AwesomeEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChannelSettings getChannelSettings (juce::AudioProcessorValueTreeState& parameters)
{
	ChannelSettings settings;
	
	settings.lowCutFreq = parameters.getRawParameterValue("Lowcut Freq") ->load();
	settings.highCutFreq = parameters.getRawParameterValue("Highcut Freq") ->load();
	settings.peakFreq = parameters.getRawParameterValue("Peak Freq") ->load();
	settings.peakGainInDecibels = parameters.getRawParameterValue("Peak Gain") ->load();
	settings.peakQuality = parameters.getRawParameterValue("Peak Quality") ->load();
	settings.lowCutSlope = static_cast<Slope>(parameters.getRawParameterValue("Lowcut Slope") ->load());
	settings.highCutSlope = static_cast<Slope>(parameters.getRawParameterValue("Highcut Slope") ->load());
	
	return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout AwesomeEQAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("Lowcut Freq", "Lowcut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20.f ));
	
	layout.add(std::make_unique<juce::AudioParameterFloat>("Highcut Freq", "Highcut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20000.f ));
	
	layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Freq", "Peak Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 750.f ));
	
	layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Gain", "Peak Gain", juce::NormalisableRange<float>(-36.f, 36.f, 0.5f, 1.f), 0.0f ));
	
	layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Quality", "Peak Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.f ));
	
	
	
	juce::StringArray stringArray;
	
	for (int i = 0; i < 4; i++)
	{
		juce::String str;
		str << (12 + i * 12);
		str << " db/Oct";
		
		stringArray.add(str);
	}
	
	layout.add (std::make_unique<juce::AudioParameterChoice>("Lowcut Slope", "Lowcut Slope", stringArray, 0));
	layout.add (std::make_unique<juce::AudioParameterChoice>("Highcut Slope", "Highcut Slope", stringArray, 0));
	
	return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AwesomeEQAudioProcessor();
}
