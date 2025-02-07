/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SerumInterfaceComponent.h"
#include "SettingsComponent.h"



//==============================================================================
/**
*/
class CGPTxSerumAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CGPTxSerumAudioProcessor();
    ~CGPTxSerumAudioProcessor() override;

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

    void CGPTxSerumAudioProcessor::listSerumParameters();

    std::function<void()> onPresetApplied;


    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================
    //std::unique_ptr<juce::AudioPluginInstance> serumInstance;
    //juce::AudioPluginInstance* getSerumInstance() const { return serumInstance.get(); }
    juce::AudioPluginInstance* getSerumInstance(); // New function to expose instance
    SerumInterfaceComponent& getSerumInterface() { return serumInterface; }
    void setSerumPath(const juce::String& newPath);
    void applyPresetToSerum(const std::map<std::string, std::string>& ChatResponse);
private:
    //==============================================================================
    std::map<std::string, int> parameterMap;
    void enumerateParameters();
    void CGPTxSerumAudioProcessor::setParameterByName(const std::pair<std::string, float>& paramData);
    float parseValue(const std::string& value);
    
    SerumInterfaceComponent serumInterface;
    SettingsComponent settingsComponent;
    juce::String serumPluginPath = "C:/Program Files/Common Files/VST3/Serum.vst3"; //change later
    /*
    //testing output with basic oscillator:
    float phase = 0.0f; // Phase of the oscillator
    float phaseIncrement = 0.0f; // Increment for phase per sample
    float frequency = 440.0f; // Frequency of the oscillator (A4)
    */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CGPTxSerumAudioProcessor)


    
};