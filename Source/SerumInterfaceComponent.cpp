#include "SerumInterfaceComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>



void SerumInterfaceComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); // Fill the background with a color
    g.setColour(juce::Colours::white);  // Set text color
    g.drawText("Serum.vst3 not detected - Please set the correct plugin path in settings.", getLocalBounds(), juce::Justification::centred, true);
}

SerumInterfaceComponent::SerumInterfaceComponent(juce::AudioProcessor& processor)
    : parentProcessor(processor)
{
    formatManager.addDefaultFormats();
    DBG("Plugin format added: " << formatManager.getFormat(0)->getName());
    juce::AudioProcessor::BusesLayout layout;
    if (!isBusesLayoutSupported(layout))
    {
        DBG("Unsupported bus layout");
        return;
    }
}

SerumInterfaceComponent::~SerumInterfaceComponent()
{
    const juce::ScopedLock lock(criticalSection); // Ensure thread-safe cleanup
    serumEditor = nullptr; 
    serumInstance = nullptr;
    //serumInstance.reset();
    //serumEditor.reset();
}

void SerumInterfaceComponent::loadSerum(const juce::File& pluginPath)
{
    if (serumInstance != nullptr)
    {
        serumEditor.reset(); // Reset the plugin editor
        serumInstance.reset(); // Release the plugin instance
        DBG("Unloaded previous plugin instance.");
    }

   


    if (!pluginPath.exists()) 
    {
        DBG("Plugin path does not exist: " << pluginPath.getFullPathName());
        return;
    }

    if (pluginPath.isDirectory())
    {
        DBG("Plugin path is a directory: " << pluginPath.getFullPathName());
    }
    else if (pluginPath.existsAsFile())
    {
        DBG("Plugin path is a file: " << pluginPath.getFullPathName());
    }
    else
    {
        DBG("Plugin path is neither a valid file nor directory: " << pluginPath.getFullPathName());
        return;
    }

    juce::AudioProcessor::BusesLayout layout;


    if (!isBusesLayoutSupported(layout))
    {
        DBG("Unsupported bus layout");
        return;
    }

    DBG("Bus layout is supported, proceeding to load plugin.");


    double sampleRate = parentProcessor.getSampleRate();
    int blockSize = 32; //parentProcessor.getBlockSize();

    DBG("Sample Rate: " << sampleRate);
    DBG("Block Size: " << blockSize);

    auto* format = formatManager.getFormat(0); // Assuming the first format is VST3
    if (format == nullptr)
    {
        DBG("No plugin formats available!");
        return;
    }


    juce::KnownPluginList pluginList;
    juce::PluginDescription pluginDescription;
    juce::String errorMessage;

    
    juce::OwnedArray<juce::PluginDescription> descriptions;
    if (!pluginList.scanAndAddFile(pluginPath.getFullPathName(),
        true, 
        descriptions,
        *format)) 
    {
        DBG("Failed to scan and add plugin: " << pluginPath.getFullPathName());
        return;
    }
    if (descriptions.isEmpty())
    {
        DBG("No plugin descriptions found!");
        return;
    }
        //auto types = pluginList.getTypes();//might be able to delete
        pluginDescription = *descriptions.getFirst();

  
        if (!pluginDescription.fileOrIdentifier.isEmpty())
        {
            DBG("Plugin Name: " << pluginDescription.name);
            DBG("Manufacturer: " << pluginDescription.manufacturerName);
            DBG("File Path: " << pluginDescription.fileOrIdentifier);
            DBG("Plugin Format: " << pluginDescription.pluginFormatName);
            DBG("Version: " << pluginDescription.version);
            DBG("Unique ID: " << pluginDescription.uniqueId);
            DBG("Category: " << pluginDescription.category);
            DBG("Is Instrument: " << (pluginDescription.isInstrument ? "true" : "false"));
        }
        else
        {
            DBG("Plugin description is invalid.");
        }

    std::unique_ptr<juce::AudioPluginInstance> instance;


    instance = formatManager.createPluginInstance(pluginDescription, sampleRate, blockSize, errorMessage);

    if (instance == nullptr)
    {

        DBG("Error loading plugin: " << errorMessage);
        return;
    }

    DBG("Plugin instance created!");
    serumInstance = std::move(instance);

    // Create the plugin editor
    serumEditor.reset(serumInstance->createEditorIfNeeded());
    if (serumEditor != nullptr)
    {
        DBG("Editor created successfully!");
        addAndMakeVisible(serumEditor.get());
        resized();
    }
    else
    {
        DBG("Failed to create plugin editor.");
    }

   
}





void SerumInterfaceComponent::resized()
{
    const juce::ScopedLock lock(criticalSection); // Synchronize access
    if (serumEditor != nullptr)
    {
        serumEditor->setBounds(getLocalBounds()); // Make Serum's UI fit the tab
    }
}

bool SerumInterfaceComponent::isBusesLayoutSupported(const juce::AudioProcessor::BusesLayout& layouts) const
{

    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput = layouts.getMainInputChannelSet();

    // Allow stereo or no input for flexibility
    if (!mainInput.isDisabled() && mainInput != mainOutput)
        return false;

    // Ensure the output supports a maximum of stereo
    if (mainOutput.size() > 2)
        return false;

    return true; // Layout is supported
}
