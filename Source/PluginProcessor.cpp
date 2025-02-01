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
        {"A UniBlend", "50"}, // incorrect
        {"A WTPos", "4"}, // needs replacement
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
        {"B WTPos", "12"}, // needs replacement
        {"B Pan", "0"},
        {"B Vol", "32%"},
        {"B Unison", "14"},
        {"B Octave", "-2 Oct"},
        {"B Semi", "+11 semitones"},
        {"B Fine", "-66 cents"},
        {"Hyp Enable", "on"},
        {"Hyp_Rate", "25%"},
        {"Hyp_Detune", "25%"},
        {"Hyp_Retrig", "on"},
        {"Hyp_Wet", "25%"},
        {"Hyp_Unison", "0"},
        {"HypDim_Size", "25%"},
        {"HypDim_Mix", "25%"},
        {"Dist Enable", "on"},
        {"Dist_Mode", "Diode 1"},
        {"Dist_PrePost", "Pre"},
        {"Dist_Freq", "8 Hz"},
        {"Dist_BW", "3"},
        {"Dist_L/B/H", "27%"},
        {"Dist_Drv", "27%"},
        {"Dist_Wet", "27%"},
        {"Flg Enable", "on"},
        {"Flg_Rate", "2.00 Hz"},
        {"Flg_BPM_Sync", "off"},
        {"Flg_Dep", "27%"},
        {"Flg_Feed", "27%"},
        {"Flg_Stereo", "27 Hz"},
        {"Flg_Wet", "27%"},
        {"Phs Enable", "on"},
        {"Phs_Rate", "2.00 Hz"},
        {"Phs_BPM_Sync", "off"},
        {"Phs_Dpth", "27%"},
        {"Phs_Frq", "27 Hz"},
        {"Phs_Feed", "27%"},
        {"Phs_Stereo", "27 deg."},
        {"Phs_Wet", "27%"},
        {"Cho Enable", "on"},
        {"Cho_Rate", "2.00 Hz"},
        {"Cho_BPM_Sync", "off"},
        {"Cho_Dly", "2.0 ms"},
        {"Cho_Dly2", "2.0 ms"},
        {"Cho_Dep", "12.6 ms"},
        {"Cho_Feed", "12%"},
        {"Cho_Filt", "227 Hz"},
        {"Cho_Wet", "27%"},
        {"Dly Enable", "on"},
        {"Dly_Feed", "27%"},
        {"Dly_BPM_Sync", "off"},
        {"Dly_Link", "Unlink"},
        {"Dly_TimL", "104.00"}, 
        {"Dly_TimR", "104.00"}, 
        {"Dly_BW", "2.7"},
        {"Dly_Freq", "270 Hz"},
        {"Dly_Mode", "TapDelay"}, 
        {"Dly_Wet", "27%"},
        {"Comp Enable", "on"},
        {"Cmp_Thr", "-6.0 dB"},
        //missing ratio? -> implement after bang functionality
        {"Cmp_Att", "30.1 ms"},
        {"Cmp_Rel", "5.1 ms"}, //not quite
        {"CmpGain", "6.0 dB"},
        {"CmpMBnd", "Multiband"},
        {"Comp_Wet", "27"}, //not working
        {"Rev Enable", "on"},
        {"VerbSize", "20%"},
        {"Decay", "2.8 s"}, 
        {"VerbLoCt", "27%"},
        {"VerbHiCt", "27%"},
        {"Spin Rate", "20%"},
        {"Verb Wet", "27%"},
        {"EQ Enable", "on"},
        {"EQ FrqL", "27 Hz"},
        {"EQ Q L", "27%"},
        {"EQ VolL", "-15.7 dB"},
        {"EQ TypL", "LPF"},
        {"EQ TypH", "Peak"}, 
        {"EQ FrqH", "28 Hz"},
        {"EQ Q H", "10%"},
        {"EQ VolH", "-12.0 dB"},
        {"FX Fil Enable", "on"},
        {"FX Fil Type", "Low 6"},
        {"FX Fil Freq", "12222 Hz"},
        {"FX Fil Reso", "10%"},
        {"FX Fil Drive", "27%"},
        {"FX Fil Pan", "27"}, //not working
        {"FX Fil Wet", "27%"}
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
