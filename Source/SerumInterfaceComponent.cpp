#include "SerumInterfaceComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>

void SerumInterfaceComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); // Fill the background with a color
    g.setColour(juce::Colours::white);  // Set text color
    g.drawText("Serum Interface Tab", getLocalBounds(), juce::Justification::centred, true);
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
    serumInstance.reset();
    serumEditor.reset();
}

void SerumInterfaceComponent::loadSerum(const juce::File& pluginPath)
{
    if (!pluginPath.exists()) // Check if the path exists at all
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

    // Check if the layout is supported
    if (!isBusesLayoutSupported(layout))
    {
        DBG("Unsupported bus layout");
        return;
    }

    DBG("Bus layout is supported, proceeding to load plugin.");


    double sampleRate = parentProcessor.getSampleRate();
    int blockSize = 512; //parentProcessor.getBlockSize();

    DBG("Sample Rate: " << sampleRate);
    DBG("Block Size: " << blockSize);

    auto* format = formatManager.getFormat(0); // Assuming the first format is VST3
    if (format == nullptr)
    {
        DBG("No plugin formats available!");
        return;
    }

    juce::PluginDescription pluginDescription;
    juce::String errorMessage;


    pluginDescription.fileOrIdentifier = pluginPath.getFullPathName();
    pluginDescription.pluginFormatName = format->getName();
    pluginDescription.name = pluginPath.getFileNameWithoutExtension(); // Set the plugin name
    pluginDescription.manufacturerName = "Unknown"; // Default value
    pluginDescription.version = "1.0.0"; // Default version
    pluginDescription.category = "Effect"; // Assume category
    pluginDescription.isInstrument = false; // Default assumption
    pluginDescription.uniqueId = 0; // No unique ID available


    // Debug output for PluginDescription
    DBG("Plugin Name: " << pluginDescription.name);
    DBG("Manufacturer: " << pluginDescription.manufacturerName);
    DBG("File Path: " << pluginDescription.fileOrIdentifier);
    DBG("Plugin Format: " << pluginDescription.pluginFormatName);
    DBG("Version: " << pluginDescription.version);
    DBG("Unique ID: " << pluginDescription.uniqueId);
    DBG("Category: " << pluginDescription.category);
    DBG("Is Instrument: " << (pluginDescription.isInstrument ? "true" : "false"));


    std::unique_ptr<juce::AudioPluginInstance> instance;
    //juce::String errorMessage;

    // Attempt to create the plugin instance directly
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

    /*
    std::function<void(std::unique_ptr<juce::AudioPluginInstance>, const juce::String&)> callback =
        [this](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error)
        {
            if (instance == nullptr)
            {
                DBG("Error loading plugin: " << error);
                return;
            }

            DBG("Plugin instance created!");
            serumInstance = std::move(instance);

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
        };

    formatManager.createPluginInstanceAsync(pluginDescription, sampleRate, blockSize, callback);
    */

    // Load the plugin instance
    /*
    formatManager.createPluginInstanceAsync(
        pluginDescription,
        sampleRate,
        blockSize,
        [this](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error)
        {
            if (instance == nullptr)
            {
                DBG("Error loading plugin: " << error);
                return;
            }

            DBG("Plugin instance created!");
            serumInstance = std::move(instance);

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
        });
        */
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
