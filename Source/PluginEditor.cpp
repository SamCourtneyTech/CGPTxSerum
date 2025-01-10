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
    : AudioProcessorEditor(&p), audioProcessor(p),
    serumInterface(p) // Pass the processor to serumInterface
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (960, 540);
    tabs.addTab("ChatGPT", juce::Colours::lightblue, &chatBar, false);
    tabs.addTab("Serum", juce::Colours::lightgreen, &serumInterface, false);
    tabs.addTab("Settings", juce::Colours::lightcoral, &settings, false);

    // Add the TabbedComponent to the editor
    addAndMakeVisible(tabs);

    // TEST: Load Serum with a hardcoded path
    //serumInterface.loadSerum(juce::File("C:/Program Files/Common Files/VST3/Serum.vst3")); //Potentially restricted
    serumInterface.loadSerum(juce::File("C:/Program Files/Common Files/VST3/Surge Synth Team/Surge XT.vst3")); //Another synth

    //serumInterface.loadSerum(juce::File("C:/OTT/OTT.vst3")); //Shouldn't be restricted

}

CGPTxSerumAudioProcessorEditor::~CGPTxSerumAudioProcessorEditor()
{
}

//==============================================================================
void CGPTxSerumAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (30.0f));
    g.drawFittedText ("ChatGPTxSerum (SC1)", getLocalBounds(), juce::Justification::centred, 1);
}

void CGPTxSerumAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    /*
    juce::File pluginPath("C:/Program Files/Common Files/VST3/OTT.vst3");
    DBG("Plugin path: " << pluginPath.getFullPathName());
    DBG("Plugin exists: " << (pluginPath.existsAsFile() ? "true" : "false"));
    DBG("Directory exists: " << (pluginPath.isDirectory() ? "true" : "false"));
    */
    tabs.setBounds(getLocalBounds());
}
