#pragma once
#include <JuceHeader.h>

class LoadingScreenComponent : public juce::Component
{
public:
    LoadingScreenComponent()
    {
        setAlwaysOnTop(true);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.7f));

        // Draw loading text
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        juce::String loadingText = "Processing request...";
        g.drawText(loadingText, getLocalBounds(), juce::Justification::centred);

        // Draw loading spinner
        float spinnerSize = 40.0f;
        float centerX = getWidth() / 2.0f;
        float centerY = getHeight() / 2.0f - 30.0f;

        float time = static_cast<float>(juce::Time::getMillisecondCounter()) / 500.0f;

        for (int i = 0; i < 8; ++i)
        {
            float angle = time + i * juce::MathConstants<float>::pi * 0.25f;
            float alpha = 0.1f + 0.7f * std::abs(std::sin(angle));

            g.setColour(juce::Colours::white.withAlpha(alpha));
            float dotSize = 8.0f;
            float radius = spinnerSize * 0.5f;
            float x = centerX + radius * std::cos(angle) - dotSize * 0.5f;
            float y = centerY + radius * std::sin(angle) - dotSize * 0.5f;
            g.fillEllipse(x, y, dotSize, dotSize);
        }

        repaint();
    }
};

class LoadingScreenManager
{
public:
    LoadingScreenManager(juce::Component* parent)
        : parentComponent(parent)
    {
        // Create the loading screen immediately but keep it invisible
        loadingScreen = std::make_unique<LoadingScreenComponent>();
        parentComponent->addChildComponent(loadingScreen.get());
        loadingScreen->setBounds(parentComponent->getLocalBounds());
        loadingScreen->setVisible(false);
        loadingScreen->setAlwaysOnTop(true);
    }

    void showLoadingScreen(bool show)
    {
        if (loadingScreen)
        {
            loadingScreen->setVisible(show);
            loadingScreen->toFront(true);  // Force it to the front when showing
            parentComponent->repaint();     // Repaint the parent
        }
    }

private:
    juce::Component* parentComponent;
    std::unique_ptr<LoadingScreenComponent> loadingScreen;
};