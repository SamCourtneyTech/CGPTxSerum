#pragma once
#include <JuceHeader.h>

class ChatBarComponent : public juce::Component
{
public:
    ChatBarComponent();
    ~ChatBarComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::TextEditor chatInput;
    juce::TextButton sendButton;
};