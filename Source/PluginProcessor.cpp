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
	auto lowcutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(channelSettings.lowCutFreq,sampleRate, (channelSettings.lowCutSlope + 1) * 2);
	auto &leftLowCut = leftChannel.get<ChannelPositions::Lowcut>();
	auto &rightLowCut = rightChannel.get<ChannelPositions::Lowcut>();
	
	updatePeakFilter(channelSettings);
	updateCutFilter(leftLowCut, lowcutCoefficients, channelSettings.lowCutSlope);
	updateCutFilter(rightLowCut, lowcutCoefficients, channelSettings.lowCutSlope);
	
	auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(channelSettings.highCutFreq, sampleRate, (channelSettings.highCutSlope + 1) * 2);
	auto &leftHighCut = leftChannel.get<ChannelPositions::HighCut>();
	auto &rightHighCut = rightChannel.get<ChannelPositions::HighCut>();
	
	updateCutFilter(leftHighCut, highCutCoefficients, channelSettings.highCutSlope);
	updateCutFilter(rightHighCut, highCutCoefficients, channelSettings.highCutSlope);
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
	updatePeakFilter(channelSettings);
	
	auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(channelSettings.lowCutFreq, getSampleRate(), (channelSettings.lowCutSlope + 1) * 2);
	auto &leftLowcut = leftChannel.get<ChannelPositions::Lowcut>();
	auto &rightLowcut = rightChannel.get<ChannelPositions::Lowcut>();
	
	updateCutFilter(leftLowcut, lowCutCoefficients, channelSettings.lowCutSlope);
	updateCutFilter(rightLowcut, lowCutCoefficients, channelSettings.lowCutSlope);
	
	auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(channelSettings.highCutFreq, getSampleRate(), (channelSettings.highCutSlope + 1) * 2);
	auto &leftHighCut = leftChannel.get<ChannelPositions::HighCut>();
	auto &rightHighCut = rightChannel.get<ChannelPositions::HighCut>();
	
	updateCutFilter(leftHighCut, highCutCoefficients, channelSettings.highCutSlope);
	updateCutFilter(rightHighCut, highCutCoefficients, channelSettings.highCutSlope);
	

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

void AwesomeEQAudioProcessor::updateCoefficients(Coefficients &old, const Coefficients &replacements)
{
	*old = *replacements;
}

void AwesomeEQAudioProcessor::updatePeakFilter(const ChannelSettings &channelSettings)
{
	auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), channelSettings.peakFreq, channelSettings.peakQuality, juce::Decibels::decibelsToGain(channelSettings.peakGainInDecibels));
	
	updateCoefficients(leftChannel.get<ChannelPositions::Peak>().coefficients , peakCoefficients);
	updateCoefficients(rightChannel.get<ChannelPositions::Peak>().coefficients , peakCoefficients);
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
