#include "ChatBarComponent.h"
#include <sstream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include <string>

class ChatBarButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
    {
        auto font = juce::Font("Press Start 2P", 15.0f, juce::Font::plain); // ChatBar-specific font
        g.setFont(font);
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(),
            juce::Justification::centred, 1);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        juce::Colour fillColour = isButtonDown ? juce::Colours::darkblue
            : isMouseOverButton ? juce::Colours::blue
            : backgroundColour;

        g.setColour(fillColour);
        g.fillRect(bounds);
    }
};



ChatBarComponent::ChatBarComponent()
{

    static ChatBarButtonLookAndFeel customSummonButton;

    addAndMakeVisible(chatInput);
    addAndMakeVisible(sendButton);
    chatInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    chatInput.setColour(juce::TextEditor::outlineColourId, juce::Colours::dimgrey);
    chatInput.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::whitesmoke);
    chatInput.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));

    //chatInput.setJustification(juce::Justification::centredLeft); 
    chatInput.setBorder(juce::BorderSize<int>(2)); // Reduce padding
    //chatInput.setBounds(20, 20, 200, 24); 

    sendButton.setButtonText("Summon");
    sendButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black); // Background color
    sendButton.setColour(juce::TextButton::textColourOnId, juce::Colours::whitesmoke);    // Text color when pressed
    sendButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white); // Default text color


    sendButton.setLookAndFeel(&customSummonButton);



}

/*
void ChatBarComponent::processChatGPTResponse(const std::string& response)
{
    std::istringstream stream(response);
    std::string token;

    while (std::getline(stream, token, ','))
    {
        std::istringstream pairStream(token);
        std::string paramName;
        float value;

        if (std::getline(pairStream, paramName, '='))
        {
            pairStream >> value;
            audioProcessor.setParameterByName(paramName, value);
        }
    }
}
void ChatBarComponent::sendToChatGPT(const std::string& userMessage)
{
    std::string chatGPTResponse = callChatGPTApi(userMessage); // Your API call function

    if (!chatGPTResponse.empty())
    {
        processChatGPTResponse(chatGPTResponse);
    }
}
*/


ChatBarComponent::~ChatBarComponent() 
{
    sendButton.setLookAndFeel(nullptr); // Reset LookAndFeel to avoid dangling pointer
}




void ChatBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void ChatBarComponent::resized()
{
    auto area = getLocalBounds();

    auto chatBarHeight = 25;    // Height of the chat input
    auto chatBarWidth = 600;    // Width of the chat input
    auto buttonWidth = 100;

    auto yPosition = (getHeight() - chatBarHeight) / 2;

    chatInput.setBounds((getWidth() - chatBarWidth - buttonWidth - 10) / 2, yPosition + -50, chatBarWidth, chatBarHeight);

    sendButton.setBounds(chatInput.getRight() + 10, yPosition -50, buttonWidth, chatBarHeight);

    //chatInput.setBounds(area.removeFromTop(30));
    
    //sendButton.setBounds(area.removeFromTop(30).reduced(5));
}


// Map for Serum parameter names and their target values
/*
std::map<std::string, std::string> ChatBarComponent::aiResponse1 = {
    {"Env1 Atk", "0.5 ms"},
    {"Env1 Hold", "0.0 ms"},
    {"Env1 Dec", "1.00 s"},
    {"Fil Cutoff", "425 Hz"},
    {"Fil Reso", "10%"}
};

// Static function to return the map
const std::map<std::string, std::string>& ChatBarComponent::getAIResponse()
{
    return aiResponse1;
}
*/