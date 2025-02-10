#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ChatBarComponent.h"
#include "SerumInterfaceComponent.h"
#include "SettingsComponent.h"
#include "LoadingComponent.h"

class CGPTxSerumAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CGPTxSerumAudioProcessorEditor (CGPTxSerumAudioProcessor&);
    ~CGPTxSerumAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    

    void CGPTxSerumAudioProcessorEditor::showLoadingScreen(bool show);
    /*
    void setLoading(bool loading)
    {
        isLoading = loading;
        repaint();  // This triggers a new paint cycle
    }
    */
private:

    bool isLoading = false;
    
    CGPTxSerumAudioProcessor& audioProcessor;


    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop }; // Create the tabbed interface

    ChatBarComponent chatBar;
    
    SettingsComponent settings;
    void loadPluginFromSettings(const juce::String& path);

    std::unique_ptr<LoadingScreenManager> loadingManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CGPTxSerumAudioProcessorEditor)
};
