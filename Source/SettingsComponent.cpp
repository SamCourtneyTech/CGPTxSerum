#include "SettingsComponent.h"
//#include "CustomButtonLookAndFeel.h"

class SettingsButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
    {
        auto font = juce::Font("Press Start 2P", 10.0f, juce::Font::plain); // Settings-specific font
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
        juce::Colour fillColour = isButtonDown ? juce::Colours::darkgrey
            : isMouseOverButton ? juce::Colours::lightgrey
            : backgroundColour;

        g.setColour(fillColour);
        g.fillRect(bounds);
    }
};




SettingsComponent::SettingsComponent(CGPTxSerumAudioProcessor& processor)
{
    // Use 'processor' here if necessary

    static SettingsButtonLookAndFeel customSettingsButtons;

    // Initialize propertiesFile
    juce::PropertiesFile::Options options;
    options.applicationName = "CGPTxSerum";
    options.filenameSuffix = ".settings";
    options.folderName = "CGPTxSerumApp";
    options.osxLibrarySubFolder = "Application Support";
    applicationProperties.setStorageParameters(options);

    // Set default path
    defaultPath = "C:/Program Files/Common Files/VST3/Serum.vst3";
    //defaultPath = "C:/Program Files";

    // Configure path label
    pathLabel.setText("Serum Path:", juce::dontSendNotification);
    pathLabel.setColour(juce::Label::textColourId, juce::Colours::indianred);
    pathLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::italic));
    addAndMakeVisible(pathLabel);


    // Configure path display
    pathDisplay.setText(loadSavedPath(), juce::dontSendNotification);
    pathDisplay.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    pathDisplay.setColour(juce::Label::textColourId, juce::Colours::white);
    pathDisplay.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pathDisplay.setJustificationType(juce::Justification::centredLeft);
    pathDisplay.setBorderSize(juce::BorderSize<int>(2));
    addAndMakeVisible(pathDisplay);

    // Configure browse button
    
    browseButton.setLookAndFeel(&customSettingsButtons);
    addAndMakeVisible(browseButton);
    browseButton.setButtonText("Browse");
    browseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke); // Background color
    browseButton.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgoldenrod);    // Text color when pressed
    browseButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black); // Default text color


    //browseButton.setColo
    browseButton.onClick = [this]() { browseForPath(); };

    // Configure reset button
    addAndMakeVisible(resetButton);
    resetButton.setButtonText("Reset Path");
    resetButton.setLookAndFeel(&customSettingsButtons);
    resetButton.onClick = [this]() { resetSavedPath(); };
    resetButton.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke); // Background color
    resetButton.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgoldenrod);    // Text color when pressed
    resetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black); // Default text color



    DBG("SettingsComponent constructed with path: " << loadSavedPath());
}

SettingsComponent::~SettingsComponent()
{
    browseButton.setLookAndFeel(nullptr);
}





void SettingsComponent::resetSavedPath()
{
    auto* userSettings = applicationProperties.getUserSettings();
    if (userSettings != nullptr)
    {
        userSettings->removeValue("pluginPath"); // Remove the saved path
        userSettings->saveIfNeeded();

        // Update the display and internal state to the default path
        pathDisplay.setText(defaultPath, juce::dontSendNotification);
        if (onPathChanged)
        {
            onPathChanged(defaultPath); // Reload Serum with the default path if needed
        }
        DBG("Path reset to default: " << defaultPath);
    }
    else
    {
        DBG("User settings not initialized.");
    }
}
//This version of the function makes you find the exact vst3 file
void SettingsComponent::browseForPath()
{
    DBG("Browse button clicked!");
    // Use native file explorer, initially targeting the default path
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select Serum.vst3 or its Containing Folder",
        juce::File(defaultPath),  // Default starting directory
        "*.vst3"                  // File filter for .vst3 files
    );

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, // Allow file selection
        [this](const juce::FileChooser& fileChooser)
        {
            DBG("FileChooser callback triggered.");

            auto selectedFileOrFolder = fileChooser.getResult();
            if (selectedFileOrFolder.exists())
            {
                if (selectedFileOrFolder.isDirectory())
                {
                    DBG("Selected directory: " << selectedFileOrFolder.getFullPathName());

                    // Check if Serum.vst3 exists in the selected directory
                    auto serumFile = selectedFileOrFolder.getChildFile("Serum.vst3");
                    if (serumFile.existsAsFile())
                    {
                        DBG("Found Serum.vst3 in directory: " << serumFile.getFullPathName());
                        pathDisplay.setText(serumFile.getFullPathName(), juce::dontSendNotification);
                        savePath(serumFile.getFullPathName());
                        if (onPathChanged)
                            onPathChanged(serumFile.getFullPathName());
                    }
                    else
                    {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "Invalid Folder",
                            "The selected folder does not contain Serum.vst3. Please select the correct folder or file."
                        );
                    }
                }
                else if (selectedFileOrFolder.getFileName() == "Serum.vst3" && selectedFileOrFolder.existsAsFile())
                {
                    DBG("Selected Serum.vst3 file: " << selectedFileOrFolder.getFullPathName());
                    pathDisplay.setText(selectedFileOrFolder.getFullPathName(), juce::dontSendNotification);
                    savePath(selectedFileOrFolder.getFullPathName());
                    if (onPathChanged)
                        onPathChanged(selectedFileOrFolder.getFullPathName());
                }
                else
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Invalid Selection",
                        "Please select the Serum.vst3 file or its containing folder."
                    );
                }
            }
            else
            {
                DBG("No valid file or folder selected, or dialog canceled.");
            }
        });
}

//This version makes it so you only have to direct to the folder containing the vst3, ex: VST3.
/*
void SettingsComponent::browseForPath()
{
    DBG("Browse button clicked!");
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select the Folder Containing Serum.vst3",
        juce::File(defaultPath),  // Default directory or file
        "*.*",                   // Allow all files and folders
        true                     // Use native dialog
    );

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
        [this](const juce::FileChooser& fileChooser)
        {
            DBG("FileChooser callback triggered.");

            auto selectedFolder = fileChooser.getResult(); // Retrieve the selected folder
            if (selectedFolder.exists())
            {
                juce::String folderPath;

                // Check if the selected folder is a `.vst3` plugin directory
                if (selectedFolder.getFileExtension() == ".vst3")
                {
                    DBG("Selected a .vst3 folder: " << selectedFolder.getFullPathName());
                    folderPath = selectedFolder.getFullPathName();
                }
                else if (selectedFolder.isDirectory()) // Regular directory containing Serum.vst3
                {
                    auto vst3Folder = selectedFolder.getChildFile("Serum.vst3");
                    if (vst3Folder.exists() && vst3Folder.isDirectory())
                    {
                        DBG("Found Serum.vst3 as a folder inside: " << vst3Folder.getFullPathName());
                        folderPath = vst3Folder.getFullPathName();
                    }
                    else
                    {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "Invalid Folder",
                            "The selected folder does not contain Serum.vst3. Please select the correct folder.");
                        return; // Exit early if no valid path
                    }
                }
                else
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Invalid Selection",
                        "Please select the Serum.vst3 folder or the directory containing it.");
                    return; // Exit early if no valid path
                }

                // Save the folder path and update the display
                pathDisplay.setText(folderPath, juce::dontSendNotification);
                savePath(folderPath);

                // Trigger the callback to reload Serum
                if (onPathChanged)
                {
                    DBG("Triggering onPathChanged callback with folder path: " << folderPath);
                    onPathChanged(folderPath);
                }
            }
            else
            {
                DBG("No valid folder selected or dialog canceled.");
            }
        });
}

*/



juce::String SettingsComponent::getPluginPath() const
{
    return pluginPathLabel.getText(); // Replace with your actual logic for retrieving the path
}






void SettingsComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); // Background
}

void SettingsComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Path label and display layout
    pathLabel.setBounds(bounds.removeFromTop(20));
    pathDisplay.setBounds(bounds.removeFromTop(30));

    // Button dimensions and spacing
    auto buttonWidth = 120;   // Width of each button
    auto buttonHeight = 30;  // Height of each button
    auto buttonSpacing = 10; // Space between buttons

    // Reserve vertical space for buttons
    auto buttonArea = bounds.removeFromTop(buttonHeight * 2 + buttonSpacing);

    // Position buttons justified to the left
    browseButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    resetButton.setBounds(buttonArea.getX() + buttonWidth + buttonSpacing, buttonArea.getY(), buttonWidth, buttonHeight);
}


void SettingsComponent::savePath(const juce::String& path)
{
    auto* userSettings = applicationProperties.getUserSettings();
    userSettings->setValue("pluginPath", path);
    userSettings->saveIfNeeded();
}

juce::String SettingsComponent::loadSavedPath()
{
    auto* userSettings = applicationProperties.getUserSettings();
    return userSettings->getValue("pluginPath", defaultPath);
}

