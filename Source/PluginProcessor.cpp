#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <map>
#include <string>
#include <juce_core/juce_core.h>
#include <regex>
#include "ParameterNormalizer.h"

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

    enumerateParameters(); // Call parameter mapping function //new 
    applyPresetToSerum();
}

void CGPTxSerumAudioProcessor::enumerateParameters()
{
    auto* serum = getSerumInstance();  // Get the Serum instance

    if (!serum) return;  // Ensure Serum is loaded

    int numParams = serum->getNumParameters();

    for (int i = 0; i < numParams; ++i)
    {
        juce::String paramName = serum->getParameterName(i);  // Use juce::String
        parameterMap[paramName.toStdString()] = i;  // Convert to std::string
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


void CGPTxSerumAudioProcessor::applyPresetToSerum()
{
    auto* serum = getSerumInstance();
    if (!serum) return;

    std::map<std::string, std::string> serumPreset = {
        {"Env1 Atk", "2.0 s"},
        {"Env1 Hold", "3.0 s"},
        {"Env1 Dec", "3.0 s"},
        {"Env1 Sus", "-inf dB"},
        {"Env1 Rel", "2.0 s"},
        {"Osc A On", "on"},
        {"A UniDet", "0.50"},
        {"A UniBlend", "50"},
        {"A WTPos", "4"},
        {"A Pan", "-22"},
        {"A Vol", "100% (0.0 dB)"},
        {"A Unison", "4"},
        {"A Octave", "+1 Oct"},
        {"A Semi", "-1 semitones"},
        {"A Fine", "24 cents"},
        {"Fil Type", "MG Low 12"},
        {"Fil Cutoff", "12222 Hz"},
        {"Fil Reso", "50%"},
        {"Filter On", "on"},
        {"Fil Driv", "0%"},
        {"Fil Var", "0%"},
        {"Fil Mix", "100%"},
        {"OscA>Fil", "on"},
        {"OscB>Fil", "on"},
        {"OscN>Fil", "on"},
        {"OscS>Fil", "on"},
        {"Osc N On", "on"},
        {"Noise Pitch", "0%"},
        {"Noise Level", "0% (-inf dB)"},
        {"Osc S On", "on"},
        {"Sub Osc Level", "0%"},
        {"SubOscOctave", "0 Oct"},
        {"SubOscShape", "Square"},
        {"Osc B On", "on"},
        {"B UniDet", "0.00"},
        {"B UniBlend", "0"},
        {"B WTPos", "12"},
        {"B Pan", "0"},
        {"B Vol", "32%"},
        {"B Unison", "14"},
        {"B Octave", "-2 Oct"},
        {"B Semi", "+11 semitones"},
        {"B Fine", "-66 cents"},
        {"Hyp Enable", "on"},
        {"Hyp_Rate", "0%"},
        {"Hyp_Detune", "0%"},
        {"Hyp_Retrig", "on"},
        {"Hyp_Wet", "0%"},
        {"Hyp_Unison", "0"},
        {"HypDim_Size", "0%"},
        {"HypDim_Mix", "0%"},
        {"Dist Enable", "on"},
        {"Dist_Mode", "Diode 1"},
        {"Dist_PrePost", "Pre"},
        {"Dist_Freq", "8 Hz"},
        {"Dist_BW", "3"},
        {"Dist_L/B/H", "0%"},
        {"Dist_Drv", "0%"},
        {"Dist_Wet", "0%"},
        {"Flg Enable", "on"},
        {"Flg_Rate", "0.00 Hz"},
        {"Flg_BPM_Sync", "on"},
        {"Flg_Dep", "0%"},
        {"Flg_Feed", "0%"},
        {"Flg_Stereo", "22 Hz"},
        {"Flg_Wet", "0%"},
        {"Phs Enable", "on"},
        {"Phs_Rate", "0.00 Hz"},
        {"Phs_BPM_Sync", "on"},
        {"Phs_Dpth", "0%"},
        {"Phs_Frq", "20 Hz"},
        {"Phs_Feed", "0%"},
        {"Phs_Stereo", "0 deg."},
        {"Phs_Wet", "0%"},
        {"Cho Enable", "on"},
        {"Cho_Rate", "0.00 Hz"},
        {"Cho_BPM_Sync", "on"},
        {"Cho_Dly", "0.0 ms"},
        {"Cho_Dly2", "0.0 ms"},
        {"Cho_Dep", "12.6 ms"},
        {"Cho_Feed", "12%"},
        {"Cho_Filt", "50 Hz"},
        {"Cho_Wet", "0%"},
        {"Dly Enable", "on"},
        {"Dly_Feed", "0%"},
        {"Dly_BPM_Sync", "on"},
        {"Dly_Link", "Unlink"},
        {"Dly_TimL", "1.00"},
        {"Dly_TimR", "1.00"},
        {"Dly_BW", "0.8"},
        {"Dly_Freq", "40 Hz"},
        {"Dly_Mode", "Normal"},
        {"Dly_Wet", "0%"},
        {"Comp Enable", "on"},
        {"Cmp_Thr", "0.0 dB"},
        {"Cmp_Att", "0.1 ms"},
        {"Cmp_Rel", "0.1 ms"},
        {"CmpGain", "0.0 dB"},
        {"CmpMBnd", "Normal"},
        {"Comp_Wet", "0"},
        {"Rev Enable", "on"},
        {"VerbSize", "0%"},
        {"Decay", "0.8 s"},
        {"VerbLoCt", "0%"},
        {"VerbHiCt", "0%"},
        {"Spin Rate", "0%"},
        {"Verb Wet", "0%"},
        {"EQ Enable", "on"},
        {"EQ FrqL", "22 Hz"},
        {"EQ Q L", "0%"},
        {"EQ VolL", "-24.0 dB"},
        {"EQ TypL", "Shelf"},
        {"EQ TypeH", "Shelf"},
        {"EQ FrqH", "22 Hz"},
        {"EQ Q H", "0%"},
        {"EQ VolH", "-24.0 dB"},
        {"FX Fil Enable", "on"},
        {"FX Fil Type", "MG Low 12"},
        {"FX Fil Freq", "8000 Hz"},
        {"FX Fil Reso", "50%"},
        {"FX Fil Drive", "0%"},
        {"FX Fil Pan", "0%"},
        {"FX Fil Wet", "0%"}
    };

    for (const auto& param : serumPreset)
    {
        const std::string& paramName = param.first;  // Extract parameter name
        const std::string& textValue = param.second; // Extract raw text value

        setParameterByName(normalizeValue(paramName, textValue));
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
