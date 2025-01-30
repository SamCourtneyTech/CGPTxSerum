/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_processors/juce_audio_processors.h> // Include this for AudioPluginInstance and FormatManager


//==============================================================================
CGPTxSerumAudioProcessor::CGPTxSerumAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    settingsComponent(*this),   // Pass the processor to settingsComponent
    serumInterface(*this)       // Pass the processor to serumInterface
{
}



CGPTxSerumAudioProcessor::~CGPTxSerumAudioProcessor()
{
}

//==============================================================================
const juce::String CGPTxSerumAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CGPTxSerumAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CGPTxSerumAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CGPTxSerumAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CGPTxSerumAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CGPTxSerumAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CGPTxSerumAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CGPTxSerumAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CGPTxSerumAudioProcessor::getProgramName (int index)
{
    return {};
}

void CGPTxSerumAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CGPTxSerumAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DBG("prepareToPlay called: SampleRate = " << sampleRate << ", BlockSize = " << samplesPerBlock);
    
    // Load Serum only once using the stored path
    if (serumInterface.getSerumInstance() == nullptr) 
    {
        serumInterface.loadSerum(juce::File(serumPluginPath));
    }

    if (serumInterface.getSerumInstance() != nullptr)
    {
        DBG("Serum loaded successfully from stored path!");
        serumInterface.prepareToPlay(sampleRate, samplesPerBlock);
    }
    else
    {
        DBG("Failed to load Serum from stored path.");
    }
}

void CGPTxSerumAudioProcessor::setSerumPath(const juce::String& newPath)
{
    if (newPath != serumPluginPath)
    {
        DBG("Updating Serum Path: " << newPath);
        serumPluginPath = newPath; // Store new path

        // Reload Serum with the new path and reconfigure audio
        serumInterface.loadSerum(juce::File(serumPluginPath));
        serumInterface.prepareToPlay(getSampleRate(), getBlockSize());
    }
}

void CGPTxSerumAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CGPTxSerumAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    DBG("Checking bus layouts...");
    DBG("Main Output Channels: " << layouts.getMainOutputChannelSet().getDescription());
    DBG("Main Input Channels: " << layouts.getMainInputChannelSet().getDescription());

#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    DBG("Plugin is a MIDI Effect.");
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    {
        DBG("Unsupported output layout.");
        return false;
    }

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    {
        DBG("Input and output layouts do not match.");
        return false;
    }
#endif

    DBG("Bus layout supported.");
    return true;
#endif
}
#endif

juce::AudioPluginInstance* CGPTxSerumAudioProcessor::getSerumInstance()
{
    return serumInterface.getSerumInstance(); // Retrieve from SerumInterfaceComponent
}



void CGPTxSerumAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Forward processing to Serum interface
    serumInterface.processMidiAndAudio(buffer, midiMessages, getSampleRate());
}



//==============================================================================
bool CGPTxSerumAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CGPTxSerumAudioProcessor::createEditor()
{
    return new CGPTxSerumAudioProcessorEditor (*this);
}

//==============================================================================
void CGPTxSerumAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CGPTxSerumAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CGPTxSerumAudioProcessor();
}
