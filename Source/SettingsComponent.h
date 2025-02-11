#pragma once
#include <JuceHeader.h>
class CGPTxSerumAudioProcessor; 
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
    void paint(juce::Graphics&) override;
    void resized() override;
    void resetSavedPath();
    std::function<void(const juce::String&)> onPathChanged;
    juce::String loadSavedPath();
    juce::String getPluginPath() const;
private:
    juce::Label pathLabel;     
    juce::Label pathDisplay;   
    juce::TextButton browseButton; 
    juce::TextButton resetButton;  
    juce::ApplicationProperties applicationProperties;
    juce::String defaultPath; 
    std::unique_ptr<juce::FileChooser> fileChooser; 
    juce::String savedPath;
    void browseForPath(); 
    void savePath(const juce::String& path); 
    juce::Label pluginPathLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};
