#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include "LoadingComponent.h"
CGPTxSerumAudioProcessorEditor::CGPTxSerumAudioProcessorEditor(CGPTxSerumAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    settings(p),
    chatBar(p),
    loadingManager(std::make_unique<LoadingScreenManager>(this))
{
    setName("CGPTxSerumAudioProcessorEditor");
    setSize(902, 760);
    tabs.addTab("ChatGPT", juce::Colours::transparentBlack, &chatBar, false);
    tabs.addTab("Serum", juce::Colours::transparentBlack, &audioProcessor.getSerumInterface(), false);
    tabs.addTab("Settings", juce::Colours::transparentBlack, &settings, false);
    settings.onPathChanged = [this](const juce::String& newPath)
        {
            DBG("onPathChanged triggered with path: " << newPath);
            audioProcessor.setSerumPath(newPath); 
        };
    addAndMakeVisible(tabs);
    audioProcessor.onPresetApplied = [this]()
        {
            tabs.setCurrentTabIndex(1); 
        };
    auto initialPath = settings.loadSavedPath();
    settings.updatePathDisplay(initialPath);
    audioProcessor.setSerumPath(initialPath); 
}
CGPTxSerumAudioProcessorEditor::~CGPTxSerumAudioProcessorEditor()
{
}
void CGPTxSerumAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f));
    g.drawFittedText("ChatGPTxSerum (SC1)", getLocalBounds(), juce::Justification::centred, 1);
    if (isLoading)  
    {
        loadingManager->showLoadingScreen(true);
        repaint(); 
    }
}
void CGPTxSerumAudioProcessorEditor::resized()
{
    tabs.setBounds(getLocalBounds());
}
void CGPTxSerumAudioProcessorEditor::loadPluginFromSettings(const juce::String& path)
{
    juce::File pluginFile(path);
    if (pluginFile.existsAsFile())
    {
        audioProcessor.setSerumPath(path); 
        DBG("Loaded plugin from: " + path);
    }
    else
    {
        DBG("Invalid plugin path: " + path);
    }
}
void CGPTxSerumAudioProcessorEditor::showLoadingScreen(bool show)
{
    if (loadingManager)
        loadingManager->showLoadingScreen(show);
}