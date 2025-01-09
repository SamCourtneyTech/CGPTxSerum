#include "ChatBarComponent.h"

ChatBarComponent::ChatBarComponent()
{
    addAndMakeVisible(chatInput);
    addAndMakeVisible(sendButton);

    sendButton.setButtonText("Send");
}

ChatBarComponent::~ChatBarComponent() {}

void ChatBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey);
}

void ChatBarComponent::resized()
{
    auto area = getLocalBounds();
    chatInput.setBounds(area.removeFromTop(30));
    sendButton.setBounds(area.removeFromTop(30).reduced(5));
}