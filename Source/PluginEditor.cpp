#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

//==============================================================================

CGPTxSerumAudioProcessorEditor::CGPTxSerumAudioProcessorEditor(CGPTxSerumAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    settings(p), chatBar(p) // Pass the processor reference to settings
{
    // Ensure size is set before construction completes
    setSize(902, 760);

    // Corrected reference to SerumInterfaceComponent
    tabs.addTab("ChatGPT", juce::Colours::transparentBlack, &chatBar, false);
    tabs.addTab("Serum", juce::Colours::transparentBlack, &audioProcessor.getSerumInterface(), false);
    tabs.addTab("Settings", juce::Colours::transparentBlack, &settings, false);

    // Update the plugin path and notify the processor
    settings.onPathChanged = [this](const juce::String& newPath)
        {
            DBG("onPathChanged triggered with path: " << newPath);
            audioProcessor.setSerumPath(newPath); // Calls processor, not SerumInterfaceComponent directly
        };



    // Add the TabbedComponent to the editor
    addAndMakeVisible(tabs);

    audioProcessor.onPresetApplied = [this]()
        {
            tabs.setCurrentTabIndex(1); // Switch to Serum tab
        };

    // Load the initial plugin path from saved settings
    auto initialPath = settings.loadSavedPath();
    settings.updatePathDisplay(initialPath);
    audioProcessor.setSerumPath(initialPath); // Calls processor, not SerumInterfaceComponent directly
}

CGPTxSerumAudioProcessorEditor::~CGPTxSerumAudioProcessorEditor()
{
}

//==============================================================================


void CGPTxSerumAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f));
    g.drawFittedText("ChatGPTxSerum (SC1)", getLocalBounds(), juce::Justification::centred, 1);
}

void CGPTxSerumAudioProcessorEditor::resized()
{
    tabs.setBounds(getLocalBounds());
}

//==============================================================================

void CGPTxSerumAudioProcessorEditor::loadPluginFromSettings(const juce::String& path)
{
    juce::File pluginFile(path);
    if (pluginFile.existsAsFile())
    {
        audioProcessor.setSerumPath(path); // Calls processor instead of directly modifying SerumInterfaceComponent
        DBG("Loaded plugin from: " + path);
    }
    else
    {
        DBG("Invalid plugin path: " + path);
    }
}
