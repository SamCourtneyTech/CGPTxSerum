#pragma once
#include <JuceHeader.h>

class CGPTxSerumAudioProcessor; // Forward declaration


class SettingsComponent : public juce::Component
{
public:
    void updatePathDisplay(const juce::String& newPath)
    {
        pathDisplay.setText(newPath, juce::dontSendNotification);
    }

    juce::String getPathDisplayText() const
    {
        return pathDisplay.getText();
    }

    explicit SettingsComponent(CGPTxSerumAudioProcessor& processor);
    ~SettingsComponent() override;

    // Component overrides
    void paint(juce::Graphics&) override;
    void resized() override;

    void resetSavedPath(); // Declare the function


    // Callback to notify about path changes
    std::function<void(const juce::String&)> onPathChanged;
    juce::String loadSavedPath();
    juce::String getPluginPath() const; // Declaration in SettingsComponent



private:
    juce::Label pathLabel;     // Label for the plugin path
    juce::Label pathDisplay;   // Display for the current plugin path
    juce::TextButton browseButton; // Button to browse for the plugin path
    juce::TextButton resetButton;  // Button to reset the path to default

    juce::ApplicationProperties applicationProperties; // For storing and loading user settings
    juce::String defaultPath; // Default plugin path

    std::unique_ptr<juce::FileChooser> fileChooser; // File chooser instance
    juce::String savedPath; // Holds the current saved path
    void browseForPath(); // Opens the file chooser
    void savePath(const juce::String& path); // Saves the plugin path
    juce::Label pluginPathLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};
