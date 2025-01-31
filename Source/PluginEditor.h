#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ChatBarComponent.h"
#include "SerumInterfaceComponent.h"
#include "SettingsComponent.h"

class CGPTxSerumAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CGPTxSerumAudioProcessorEditor (CGPTxSerumAudioProcessor&);
    ~CGPTxSerumAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    CGPTxSerumAudioProcessor& audioProcessor;


    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop }; // Create the tabbed interface

    ChatBarComponent chatBar;
    
    SettingsComponent settings;
    void loadPluginFromSettings(const juce::String& path);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CGPTxSerumAudioProcessorEditor)
};
