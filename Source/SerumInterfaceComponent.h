#pragma once


#include <JuceHeader.h>

class SerumInterfaceComponent : public juce::Component
{
public:
    SerumInterfaceComponent();
    ~SerumInterfaceComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerumInterfaceComponent)
};
