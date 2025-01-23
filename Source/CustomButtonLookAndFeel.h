#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
    {
        auto font = juce::Font("Press Start 2P", 242.0f, juce::Font::FontStyleFlags::plain);
        g.setFont(font);
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();

        juce::Colour fillColour = isButtonDown ? juce::Colours::darkgrey
            : isMouseOverButton ? juce::Colours::grey
            : backgroundColour;

        g.setColour(fillColour);
        g.fillRect(bounds);
    }
};
