/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

//==============================================================================
CGPTxSerumAudioProcessorEditor::CGPTxSerumAudioProcessorEditor(CGPTxSerumAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    serumInterface(p),
    settings(p) // Pass the processor reference to settings
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(902, 760);

    tabs.addTab("ChatGPT", juce::Colours::lightblue, &chatBar, false);
    tabs.addTab("Serum", juce::Colours::lightgreen, &serumInterface, false);
    tabs.addTab("Settings", juce::Colours::lightcoral, &settings, false);
    settings.onPathChanged = [this](const juce::String& newPath)
        {
            DBG("onPathChanged triggered with path: " << newPath);
            serumInterface.loadSerum(juce::File(newPath)); // Reload Serum with the new path
        };
    // Add the TabbedComponent to the editor
    addAndMakeVisible(tabs);

    /*
    settings.onPathChanged = [this](const juce::String& newPath)
        {
            loadPluginFromSettings(newPath); // Dynamically load the plugin with the updated path
        };
    */
    // Load the initial plugin path from saved settings or default
    auto initialPath = settings.loadSavedPath();
    settings.updatePathDisplay(initialPath); // Use the new public method
    serumInterface.loadSerum(juce::File(initialPath)); // Load Serum on startup
    //loadPluginFromSettings(initialPath);
}

CGPTxSerumAudioProcessorEditor::~CGPTxSerumAudioProcessorEditor()
{
}

//==============================================================================
void CGPTxSerumAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f));
    g.drawFittedText("ChatGPTxSerum (SC1)", getLocalBounds(), juce::Justification::centred, 1);
}

void CGPTxSerumAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor.
    tabs.setBounds(getLocalBounds());
}

//==============================================================================
void CGPTxSerumAudioProcessorEditor::loadPluginFromSettings(const juce::String& path)
{
    juce::File pluginFile(path);
    if (pluginFile.existsAsFile())
    {
        serumInterface.loadSerum(pluginFile);
        DBG("Loaded plugin from: " + path);
    }
    else
    {
        DBG("Invalid plugin path: " + path);
    }
}
