#include "SettingsComponent.h"

SettingsComponent::SettingsComponent()
{
    // Add and configure path label
    pathLabel.setText("Plugin Path:", juce::dontSendNotification);
    addAndMakeVisible(pathLabel);

    // Add and configure path input
    addAndMakeVisible(pathInput);
    pathInput.setText("Enter the path to Serum here...");
    pathInput.setMultiLine(false);

    // Add and configure browse button
    addAndMakeVisible(browseButton);
    browseButton.setButtonText("Browse");
    browseButton.onClick = [this]() { browseForPath(); };
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

void SettingsComponent::browseForPath()
{
    juce::FileChooser chooser("Select the Serum Plugin Path", juce::File(), "*.*");

    // Launch the file chooser asynchronously
    chooser.launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fileChooser) // Lambda callback
        {
            auto selectedFile = fileChooser.getResult(); // Get the selected file
            if (selectedFile.existsAsFile())
            {
                pathInput.setText(selectedFile.getFullPathName()); // Display the file path
            }
        });
}
