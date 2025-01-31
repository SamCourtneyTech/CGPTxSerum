#pragma once
#include <JuceHeader.h>


class ChatBarComponent : public juce::Component
{
public:
    ChatBarComponent();
    ~ChatBarComponent() override;
    //static const std::map<std::string, std::string>& getAIResponse();
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //static std::map<std::string, std::string> aiResponse1;  
    juce::TextEditor chatInput;
    juce::TextButton sendButton;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatBarComponent)
};