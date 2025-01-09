#include "SerumInterfaceComponent.h"

void SerumInterfaceComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); // Fill the background with a color
    g.setColour(juce::Colours::white);  // Set text color
    g.drawText("Serum Interface Tab", getLocalBounds(), juce::Justification::centred, true);
}

SerumInterfaceComponent::SerumInterfaceComponent()
{
    // Constructor: Minimal initialization
    setSize(400, 300); // Set a default size for the component
}

SerumInterfaceComponent::~SerumInterfaceComponent()
{
    // Destructor: No special cleanup needed in this shell version
}

void SerumInterfaceComponent::resized()
{
    // Placeholder for resizing logic, currently nothing to resize
}
