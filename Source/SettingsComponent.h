#pragma once
#include <JuceHeader.h>

class SettingsComponent : public juce::Component
{
public:
    SettingsComponent();
    ~SettingsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Path selection components
    juce::Label pathLabel;
    juce::TextEditor pathInput;
    juce::TextButton browseButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // Default plugin path
    juce::String defaultPath;

    // juce::ApplicationProperties for persistence
    juce::ApplicationProperties applicationProperties;

    // Path persistence functions
    void browseForPath();
    void savePath(const juce::String& path);
    juce::String loadSavedPath();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};
