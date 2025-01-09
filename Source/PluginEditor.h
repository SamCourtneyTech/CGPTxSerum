/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ChatBarComponent.h"
#include "SerumInterfaceComponent.h"
#include "SettingsComponent.h"



//==============================================================================
/**
*/
class CGPTxSerumAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CGPTxSerumAudioProcessorEditor (CGPTxSerumAudioProcessor&);
    ~CGPTxSerumAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CGPTxSerumAudioProcessor& audioProcessor;
    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop }; // Create the tabbed interface

    ChatBarComponent chatBar;
    SerumInterfaceComponent serumInterface;
    SettingsComponent settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CGPTxSerumAudioProcessorEditor)
};
