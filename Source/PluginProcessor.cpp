#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <map>
#include <string>
#include <juce_core/juce_core.h>
#include <regex>
#include "ParameterNormalizer.h"
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
    settingsComponent(*this),   
    serumInterface(*this)    
{
}
CGPTxSerumAudioProcessor::~CGPTxSerumAudioProcessor()
{
}
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
    return 1;   
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
void CGPTxSerumAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DBG("prepareToPlay called: SampleRate = " << sampleRate << ", BlockSize = " << samplesPerBlock);
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
    enumerateParameters(); 
}
void CGPTxSerumAudioProcessor::enumerateParameters()
{
    auto* serum = getSerumInstance();  
    if (!serum) return;  
    int numParams = serum->getNumParameters();
    for (int i = 0; i < numParams; ++i)
    {
        juce::String paramName = serum->getParameterName(i);  
        parameterMap[paramName.toStdString()] = i;  
    }
    DBG("Enumerated " << numParams << " parameters from Serum.");
}
void CGPTxSerumAudioProcessor::setParameterByName(const std::pair<std::string, float>& paramData) {
    const std::string& paramName = paramData.first;
    float value = paramData.second;
    auto* serum = getSerumInstance();
    if (!serum) return;
    auto it = parameterMap.find(paramName);
    if (it != parameterMap.end()) {
        int paramIndex = it->second;
        serum->setParameterNotifyingHost(paramIndex, std::clamp(value, 0.0f, 1.0f));
        DBG("Set " << paramName << " to " << value);
    }
    else {
        DBG("Parameter " << paramName << " not found.");
    }
}
float CGPTxSerumAudioProcessor::parseValue(const std::string& value)
{
    std::regex numberPattern(R"([-+]?\d*\.?\d+)");
    std::smatch match;
    if (std::regex_search(value, match, numberPattern))
    {
        float numericValue = std::stof(match.str());
        if (value.find("%") != std::string::npos)
            return numericValue / 100.0f;
        return numericValue;
    }
    return 0.0f;
}
void CGPTxSerumAudioProcessor::applyPresetToSerum(const std::map<std::string, std::string>& ChatResponse)
{
    //listSerumParameters();
    auto* serum = getSerumInstance();
    if (!serum) return;
    for (const auto& param : ChatResponse) 
    {
        const std::string& paramName = param.first;  
        const std::string& textValue = param.second;
        setParameterByName(normalizeValue(paramName, textValue));
    }
    if (onPresetApplied)
    {
        onPresetApplied();
    }
    
}
void CGPTxSerumAudioProcessor::setSerumPath(const juce::String& newPath)
{
    if (newPath != serumPluginPath)
    {
        DBG("Updating Serum Path: " << newPath);
        serumPluginPath = newPath; 
        serumInterface.loadSerum(juce::File(serumPluginPath));
        serumInterface.prepareToPlay(getSampleRate(), getBlockSize());
    }
}
void CGPTxSerumAudioProcessor::releaseResources()
{
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
    return serumInterface.getSerumInstance();
}
void CGPTxSerumAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    serumInterface.processMidiAndAudio(buffer, midiMessages, getSampleRate());
}
bool CGPTxSerumAudioProcessor::hasEditor() const
{
    return true; 
}
juce::AudioProcessorEditor* CGPTxSerumAudioProcessor::createEditor()
{
    return new CGPTxSerumAudioProcessorEditor (*this);
}
void CGPTxSerumAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void CGPTxSerumAudioProcessor::setResponses(const std::vector<std::map<std::string, std::string>>& newResponses)
{
    juce::ScopedLock lock(responseLock);
    responses = newResponses;
    currentResponseIndex = 0; 
    if (!responses.empty())
        applyPresetToSerum(responses[currentResponseIndex]);
}

void CGPTxSerumAudioProcessor::applyResponseAtIndex(int index)
{
    juce::ScopedLock lock(responseLock);
    if (index >= 0 && index < responses.size())
    {
        currentResponseIndex = index;
        applyPresetToSerum(responses[currentResponseIndex]);
    }
}

void CGPTxSerumAudioProcessor::nextResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex < responses.size() - 1)
    {
        currentResponseIndex++;
        applyPresetToSerum(responses[currentResponseIndex]);
    }
}

void CGPTxSerumAudioProcessor::previousResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex > 0)
    {
        currentResponseIndex--;
        applyPresetToSerum(responses[currentResponseIndex]);
    }
}

void CGPTxSerumAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CGPTxSerumAudioProcessor();
}
void CGPTxSerumAudioProcessor::listSerumParameters()
{
    auto* serum = getSerumInstance();
    if (serum == nullptr)
    {
        DBG("Serum instance not loaded.");
        return;
    }
    int numParams = serum->getNumParameters();
    DBG("Listing Serum Parameters (" << numParams << " total):");
    for (int i = 0; i < numParams; ++i)
    {
        juce::String paramName = serum->getParameterName(i);
        float paramValue = serum->getParameter(i);
        DBG("[" << i << "] " << paramName << " = " << paramValue);
    }
}