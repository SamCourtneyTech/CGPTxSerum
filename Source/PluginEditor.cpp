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
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set the editor's size
    setSize(960, 540);

    // Add tabs for different components
    tabs.addTab("ChatGPT", juce::Colours::lightblue, &chatBar, false);
    tabs.addTab("Serum", juce::Colours::lightgreen, &serumInterface, false);
    tabs.addTab("Settings", juce::Colours::lightcoral, &settings, false);

    // Add the TabbedComponent to the editor
    addAndMakeVisible(tabs);
}

CGPTxSerumAudioProcessorEditor::~CGPTxSerumAudioProcessorEditor()
{
    // Destructor: Nothing special to clean up
}

//==============================================================================
void CGPTxSerumAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Draw a header text
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f));
    g.drawFittedText("ChatGPTxSerum (SC1)", getLocalBounds(), juce::Justification::centred, 1);
}

void CGPTxSerumAudioProcessorEditor::resized()
{
    // Set bounds for the tabbed component
    tabs.setBounds(getLocalBounds());
}
