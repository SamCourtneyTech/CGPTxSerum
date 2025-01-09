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

    // Callback for browse button
    void browseForPath();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};
