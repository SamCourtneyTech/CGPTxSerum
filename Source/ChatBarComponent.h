#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"  // Include the processor header
#include <map>
#include <string>

class ChatBarComponent : public juce::Component
{
public:
    ChatBarComponent(CGPTxSerumAudioProcessor& p);
    ~ChatBarComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    std::map<std::string, std::string> sendPromptToChatGPT(const juce::String& userPrompt);  
    void sendAIResponseToProcessor(const std::map<std::string, std::string>& aiResponse);
    
   

private:

    CGPTxSerumAudioProcessor& processor; 
    juce::TextEditor chatInput;
    juce::TextButton sendButton;
    bool requestInProgress = false;
    
    
  

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatBarComponent)
};
