#include "SettingsComponent.h"

SettingsComponent::SettingsComponent()
{
    // Initialize propertiesFile
    juce::PropertiesFile::Options options;
    options.applicationName = "CGPTxSerum";
    options.filenameSuffix = ".settings";
    options.folderName = "CGPTxSerumApp";
    options.osxLibrarySubFolder = "Application Support";
    applicationProperties.setStorageParameters(options);

    // Set default path
    defaultPath = "C:/Program Files/Common Files/VST3/Serum.vst3";

    // Add and configure path label
    pathLabel.setText("Plugin Path:", juce::dontSendNotification);
    addAndMakeVisible(pathLabel);

    // Add and configure path input
    addAndMakeVisible(pathInput);
    pathInput.setMultiLine(false);
    pathInput.setText(loadSavedPath()); // Load saved path or default

    // Add and configure browse button
    addAndMakeVisible(browseButton);
    browseButton.setButtonText("Browse");
    browseButton.onClick = [this]() { browseForPath(); };
}
void SettingsComponent::browseForPath()
{
    DBG("Browse button clicked!");
    // Create a FileChooser instance
    fileChooser = std::make_unique<juce::FileChooser>("Select the Serum Plugin Path",
        juce::File(defaultPath), // Default directory or file
        "*.vst3",                // File extension filter
        false);    // File extension filter

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fileChooser)
        {
            DBG("FileChooser callback triggered."); // Debug to confirm callback execution

            auto selectedFile = fileChooser.getResult(); // Retrieve the selected file
            if (selectedFile.existsAsFile())            // Validate the file
            {
                pathInput.setText(selectedFile.getFullPathName()); // Update the TextEditor
                savePath(selectedFile.getFullPathName());         // Save the path
                DBG("File selected: " + selectedFile.getFullPathName());
            }
            else
            {
                DBG("No valid file selected or dialog canceled.");
            }
        });
}


SettingsComponent::~SettingsComponent() {}

void SettingsComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey); // Background
}

void SettingsComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    pathLabel.setBounds(area.removeFromTop(20));
    pathInput.setBounds(area.removeFromTop(30));
    browseButton.setBounds(area.removeFromTop(30).reduced(5));
}


void SettingsComponent::savePath(const juce::String& path)
{
    auto* userSettings = applicationProperties.getUserSettings();
    userSettings->setValue("pluginPath", path);
    userSettings->saveIfNeeded();
}

juce::String SettingsComponent::loadSavedPath()
{
    auto* userSettings = applicationProperties.getUserSettings();
    return userSettings->getValue("pluginPath", defaultPath);
}
