#include "ChatBarComponent.h"
#include <sstream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include <string>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "PluginProcessor.h" 
#include "PluginEditor.h"


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





std::map<std::string, std::string> LocalaiResponse = {
        {"Env1 Atk", "2.0 s"},
        {"Env1 Hold", "3.0 s"},
        {"Env1 Dec", "3.0 s"},
        {"Env1 Sus", "-inf dB"},
        {"Env1 Rel", "2.0 s"},
        {"Osc A On", "on"},
        {"A UniDet", "0.50"},
        {"A UniBlend", "50"},
        {"A WTPos", "4"},
        {"A Pan", "-22"},
        {"A Vol", "100% (0.0 dB)"},
        {"A Unison", "4"},
        {"A Octave", "+1 Oct"},
        {"A Semi", "-1 semitones"},
        {"A Fine", "24 cents"},
        {"Fil Type", "MG Low 12"},
        {"Fil Cutoff", "12222 Hz"},
        {"Fil Reso", "50%"},
        {"Filter On", "on"},
        {"Fil Driv", "0%"},
        {"Fil Var", "0%"},
        {"Fil Mix", "100%"},
        {"OscA>Fil", "on"},
        {"OscB>Fil", "on"},
        {"OscN>Fil", "on"},
        {"OscS>Fil", "on"},
        {"Osc N On", "on"},
        {"Noise Pitch", "0%"},
        {"Noise Level", "0% (-inf dB)"},
        {"Osc S On", "on"},
        {"Sub Osc Level", "0%"},
        {"SubOscOctave", "-4 Oct"},
        {"SubOscShape", "Square"},
        {"Osc B On", "on"},
        {"B UniDet", "0.00"},
        {"B UniBlend", "0"},
        {"B WTPos", "12"},
        {"B Pan", "0"},
        {"B Vol", "32%"},
        {"B Unison", "14"},
        {"B Octave", "-2 Oct"},
        {"B Semi", "+11 semitones"},
        {"B Fine", "66 cents"},
        {"Hyp Enable", "on"},
        {"Hyp_Rate", "25%"},
        {"Hyp_Detune", "25%"},
        {"Hyp_Retrig", "on"},
        {"Hyp_Wet", "25%"},
        {"Hyp_Unison", "0"},
        {"HypDim_Size", "25%"},
        {"HypDim_Mix", "25%"},
        {"Dist Enable", "on"},
        {"Dist_Mode", "Diode 1"},
        {"Dist_PrePost", "Pre"},
        {"Dist_Freq", "8 Hz"},
        {"Dist_BW", "3"},
        {"Dist_L/B/H", "27%"},
        {"Dist_Drv", "27%"},
        {"Dist_Wet", "27%"},
        {"Flg Enable", "on"},
        {"Flg_Rate", "2.00 Hz"},
        {"Flg_BPM_Sync", "off"},
        {"Flg_Dep", "27%"},
        {"Flg_Feed", "27%"},
        {"Flg_Stereo", "27 Hz"},
        {"Flg_Wet", "27%"},
        {"Phs Enable", "on"},
        {"Phs_Rate", "2.00 Hz"},
        {"Phs_BPM_Sync", "off"},
        {"Phs_Dpth", "27%"},
        {"Phs_Frq", "27 Hz"},
        {"Phs_Feed", "27%"},
        {"Phs_Stereo", "27 deg."},
        {"Phs_Wet", "27%"},
        {"Cho Enable", "on"},
        {"Cho_Rate", "2.00 Hz"},
        {"Cho_BPM_Sync", "off"},
        {"Cho_Dly", "2.0 ms"},
        {"Cho_Dly2", "2.0 ms"},
        {"Cho_Dep", "12.6 ms"},
        {"Cho_Feed", "12%"},
        {"Cho_Filt", "227 Hz"},
        {"Cho_Wet", "27%"},
        {"Dly Enable", "on"},
        {"Dly_Feed", "27%"},
        {"Dly_BPM_Sync", "off"},
        {"Dly_Link", "Unlink"},
        {"Dly_TimL", "104.00"},
        {"Dly_TimR", "104.00"},
        {"Dly_BW", "2.7"},
        {"Dly_Freq", "270 Hz"},
        {"Dly_Mode", "TapDelay"},
        {"Dly_Wet", "27%"},
        {"Comp Enable", "on"},
        {"Cmp_Thr", "-6.0 dB"},
        {"Cmp_Att", "30.1 ms"},
        {"Cmp_Rel", "5.1 ms"},
        {"CmpGain", "6.0 dB"},
        {"CmpMBnd", "Multiband"},
        {"Comp_Wet", "27"},
        {"Rev Enable", "on"},
        {"VerbSize", "20%"},
        {"Decay", "2.8 s"},
        {"VerbLoCt", "27%"},
        {"VerbHiCt", "27%"},
        {"Spin Rate", "20%"},
        {"Verb Wet", "27%"},
        {"EQ Enable", "on"},
        {"EQ FrqL", "27 Hz"},
        {"EQ Q L", "27%"},
        {"EQ VolL", "-15.7 dB"},
        {"EQ TypL", "LPF"},
        {"EQ TypH", "Peak"},
        {"EQ FrqH", "28 Hz"},
        {"EQ Q H", "10%"},
        {"EQ VolH", "-12.0 dB"},
        {"FX Fil Enable", "on"},
        {"FX Fil Type", "Low 6"},
        {"FX Fil Freq", "12222 Hz"},
        {"FX Fil Reso", "10%"},
        {"FX Fil Drive", "27%"},
        {"FX Fil Pan", "27"},
        {"FX Fil Wet", "27%"}
};






ChatBarComponent::ChatBarComponent(CGPTxSerumAudioProcessor& p) : processor(p)
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

    

    sendButton.onClick = [this]() {
        juce::String userInput = chatInput.getText();

        



        //processor.applyPresetToSerum(LocalaiResponse);
        
        processor.applyPresetToSerum(sendPromptToChatGPT(userInput));
        };



}




std::map<std::string, std::string> ChatBarComponent::sendPromptToChatGPT(const juce::String& userPrompt)
{
    
    
    auto* firstParent = getParentComponent();
    if (firstParent)
    {
        auto* secondParent = firstParent->getParentComponent();
        if (secondParent)
        {
            if (auto* editor = dynamic_cast<CGPTxSerumAudioProcessorEditor*>(secondParent))
            {
                DBG("Found editor");
                editor->setLoading(true);
                DBG("Set loading to true");
                
                
            }
            else
            {
                DBG("Second parent found but cast failed");
                DBG("Second parent type: " + secondParent->getName());
            }
        }
        else
        {
            DBG("No second parent found");
        }
    }
    else
    {
        DBG("No first parent found");
    }
    

    const juce::String apiEndpoint = "https://api.openai.com/v1/chat/completions";
    const juce::String apiKey = "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";

    constexpr int MAX_OPENAI_LENGTH = 32000;
    constexpr int CONNECTION_TIMEOUT_MS = 60000;  // 60 second connection timeout
    constexpr int READ_TIMEOUT_MS = 90000;

    // [All your existing system instruction and API code stays exactly the same until the final parsing]
    juce::String systemInstruction = R"(Interpret the user's request with creativity within the specified ranges and default values, leveraging sound design knowledge to produce engaging and innovative soundscapes using the provided Serum VST parameters. While every response should include all 123 parameters in order formatted as {"Parameter Name", "Value"} in a consistent list, allow for variations that reflect musicality and style. The user's request will be inputted at the bottom of this prompt. Follow these guidelines: Use the full spectrum of provided values and descriptions to address specific or abstract prompts (e.g., "bright and plucky," "deep and textured") while staying within bounds. Be imaginative in assigning values to create sound textures that meet the user's description, but adhere strictly to parameter names and ensure all 123 parameters are included every time. Return the parameters in the format {"Parameter Name", "Value"}, even if a parameter's default value remains unchanged. Here are the 123 parameters and their default values: {{"Env1 Atk", "0.5 ms"}, {"Env1 Hold", "0.0 ms"}, {"Env1 Dec", "1.00 s"}, {"Env1 Sus", "0.0 dB"}, {"Env1 Rel", "15 ms"}, {"Osc A On", "on"}, {"A UniDet", "0.25"}, {"A UniBlend", "75"}, {"A WTPos", "Sine"}, {"A Pan", "0"}, {"A Vol", "75%"}, {"A Unison", "1"}, {"A Octave", "0 Oct"}, {"A Semi", "0 semitones"}, {"A Fine", "0 cents"}, {"Fil Type", "MG Low 12"}, {"Fil Cutoff", "425 Hz"}, {"Fil Reso", "10%"}, {"Filter On", "off"}, {"Fil Driv", "0%"}, {"Fil Var", "0%"}, {"Fil Mix", "100%"}, {"OscA>Fil", "on"}, {"OscB>Fil", "off"}, {"OscN>Fil", "off"}, {"OscS>Fil", "off"}, {"Osc N On", "off"}, {"Noise Pitch", "50%"}, {"Noise Level", "25%"}, {"Osc S On", "off"}, {"Sub Osc Level", "75%"}, {"SubOscOctave", "0 Oct"}, {"SubOscShape", "Sine"}, {"Osc B On", "off"}, {"B UniDet", "0.25"}, {"B UniBlend", "75"}, {"B WTPos", "1"}, {"B Pan", "0"}, {"B Vol", "75%"}, {"B Unison", "1"}, {"B Octave", "0 Oct"}, {"B Semi", "0 semitones"}, {"B Fine", "0 cents"}, {"Hyp Enable", "off"}, {"Hyp_Rate", "40%"}, {"Hyp_Detune", "25%"}, {"Hyp_Retrig", "off"}, {"Hyp_Wet", "50%"}, {"Hyp_Unision", "4"}, {"HypDim_Size", "50%"}, {"HypDim_Mix", "0%"}, {"Dist Enable", "off"}, {"Dist_Mode", "Tube"}, {"Dist_PrePost", "Off"}, {"Dist_Freq", "330 Hz"}, {"Dist_BW", "1.9"}, {"Dist_L/B/H", "0%"}, {"Dist_Drv", "25%"}, {"Dist_Wet", "100%"}, {"Flg Enable", "off"}, {"Flg_Rate", "0.08 Hz"}, {"Flg_BPM_Sync", "off"}, {"Flg_Dep", "100%"}, {"Flg_Feed", "50%"}, {"Flg_Stereo", "180deg."}, {"Flg_Wet", "100%"}, {"Phs Enable", "off"}, {"Phs_Rate", "0.08 Hz"}, {"Phs_BPM_Sync", "off"}, {"Phs_Dpth", "50%"}, {"Phs_Frq", "600 Hz"}, {"Phs_Feed", "80%"}, {"Phs_Stereo", "180deg."}, {"Phs_Wet", "100%"}, {"Cho Enable", "off"}, {"Cho_Rate", "0.08 Hz"}, {"Cho_BPM_Sync", "off"}, {"Cho_Dly", "5.0 ms"}, {"Cho_Dly2", "0.0 ms"}, {"Cho_Dep", "26.0 ms"}, {"Cho_Feed", "10%"}, {"Cho_Filt", "1000 Hz"}, {"Cho_Wet", "50%"}, {"Dly Enable", "off"}, {"Dly_Feed", "40%"}, {"Dly_BPM_Sync", "on"}, {"Dly_Link", "Unlink, Link"}, {"Dly_TimL", "1/4"}, {"Dly_TimR", "1/4"}, {"Dly_BW", "6.8"}, {"Dly_Freq", "849 Hz"}, {"Dly_Mode", "Normal"}, {"Dly_Wet", "30%"}, {"Comp Enable", "off"}, {"Cmp_Thr", "-18.1 dB"}, {"Cmp_Att", "90.1 ms"}, {"Cmp_Rel", "90 ms"}, {"CmpGain", "0.0 dB"}, {"CmpMBnd", "Normal"}, {"Comp_Wet", "100"}, {"Rev Enable", "off"}, {"VerbSize", "35%"}, {"Decay", "4.7 s"}, {"VerbLoCt", "0%"}, {"VerbHiCt", "35%"}, {"Spin Rate", "25%"}, {"Verb Wet", "20%"}, {"EQ Enable", "off"}, {"EQ FrqL", "210 Hz"}, {"EQ Q L", "60%"}, {"EQ VolL", "0.0 dB"}, {"EQ TypL", "Shelf"}, {"EQ TypeH", "Shelf"}, {"EQ FrqH", "2041 Hz"}, {"EQ Q H", "60%"}, {"EQ VolH", "0.0"}, {"FX Fil Enable", "off"}, {"FX Fil Type", "MG Low 6"}, {"FX Fil Freq", "330 Hz"}, {"FX Fil Reso", "0%"}, {"FX Fil Drive", "0%"}, {"FX Fil Pan", "50%"}, {"FX Fil Wet", "100%"}} Here are those 123 parameter's respective ranges that you can choose from: {{"Env1 Atk", "0.0 ms - 32.0 s"}, {"Env1 Hold", "0.0 ms - 32.0 s"}, {"Env1 Dec", "0.0 ms - 32.0 s"}, {"Env1 Sus", "-inf dB - 0.0 dB"}, {"Env1 Rel", "0.0ms - 32.0s"}, {"Osc A On", "off, on"}, {"A UniDet", "0.00 - 1.00"}, {"A UniBlend", "0 - 100"}, {"A WTPos", "Sine, Saw, Triangle, Square, Pulse, Half Pulse, Inv-Phase Saw"}, {"A Pan", "-50 - 50"}, {"A Vol", "0% - 100%"}, {"A Unison", "1 - 16"}, {"A Octave", "-4 Oct, -3 Oct, -2 Oct, -1 Oct, 0 Oct, 1 Oct, 2 Oct, 3 Oct, 4 Oct"}, {"A Semi", "-12 semitones - +12 semitones"}, {"A Fine", "-100 cents - 100 cents"}, {"Fil Type", "MG Low 6, MG Low 12, MG Low 18, MG Low 24, Low 6, Low 12, Low 18, Low 24, High 6, High 12, High 18, High 24, Band 12, Band 24, Peak 12, Peak 24, Notch 12, Notch 24, LH 6, LH 12, LB 12, LP 12, LN 12, HB 12, HP 12, HN 12, BP 12, PP 12, PN 12, NN 12, L/B/H 12, L/B/H 24, L/P/H 12, L/P/H 24, L/N/H 12, L/N/H 24, B/P/N 12, B/P/N 24, Cmb +, Cmb -, Cmb L6+, Cmb L6-, Cmb H6+, Cmb H6-, Cmb HL6+, Cmb HL6-, Flg +, Flg -, Flg L6+, Flg L6-, Flg H6+, Flg H6-, Flg HL6+, Flg HL6-, Phs 12+, Phs 12-, Phs 24+, Phs 24-, Phs 36+, Phs 36-, Phs 48+, Phs 48-, Phs 48L6+, Phs 48L6-, Phs 48H6+, Phs 48H6-, Phs 48HL6+, Phs 48HL6-, FPhs 12HL6+, FPhs 12HL6-, Low EQ 6, Low EQ 12, Band EQ 12, High EQ 6, High EQ 12, Ring Mod, Ring Modx2, SampHold, SampHold-, Combs, Allpasses, Reverb, French LP, German LP, Add Bass, Formant-I, Formant-II, Formant-III, Bandreject, Dist.Comb 1 LP, Dist.Comb 1 BP, Dist.Comb 2 LP, Dist.Comb 2 BP, Scream LP, Scream BP"}, {"Fil Cutoff", "8 Hz - 22050 Hz"}, {"Fil Reso", "0% - 100%"}, {"Filter On", "off, on"}, {"Fil Driv", "0% - 100%"}, {"Fil Var", "0% - 100%"}, {"Fil Mix", "0% - 100%"}, {"OscA>Fil", "off, on"}, {"OscB>Fil", "off, on"}, {"OscN>Fil", "off, on"}, {"OscS>Fil", "off, on"}, {"Osc N On", "off, on"}, {"Noise Pitch", "0% - 100%"}, {"Noise Level", "0% - 100%"}, {"Osc S On", "off, on"}, {"Sub Osc Level", "0%-100%"}, {"SubOscOctave", "-4 Oct, -3 Oct, -2 Oct, -1 Oct, 0 Oct, 1 Oct, 2 Oct, 3 Oct, 4 Oct"}, {"SubOscShape", "Sine, RoundRect, Triangle, Saw, Square, Pulse"}, {"Osc B On", "off, on"}, {"B UniDet", "0.00 - 1.00"}, {"B UniBlend", "0 - 100"}, {"B WTPos", "Sine, Saw, Triangle, Square, Pulse, Half Pulse, Inv-Phase Saw"}, {"B Pan", "-50 - 50"}, {"B Vol", "0% - 100%"}, {"B Unison", "1 - 16"}, {"B Octave", "-4 Oct, -3 Oct, -2 Oct, -1 Oct, 0 Oct, 1 Oct, 2 Oct, 3 Oct, 4 Oct"}, {"B Semi", "-12 semitones - +12 semitones"}, {"B Fine", "-100 cents - 100 cents"}, {"Hyp Enable", "off, on"}, {"Hyp_Rate", "0% - 100%"}, {"Hyp_Detune", "0% - 100%"}, {"Hyp_Retrig", "off - Retrig"}, {"Hyp_Wet", "0% - 100%"}, {"Hyp_Unision", "0 - 7"}, {"HypDim_Size", "0% - 100%"}, {"HypDim_Mix", "0% - 100%"}, {"Dist Enable", "off, on"}, {"Dist_Mode", "Tube, SoftClip, HardClip, Diode 1, Diode 2, Lin.Fold, Sin Fold, Zero-Square, Downsample, Asym, Rectify, X-Shaper, X-Shaper (Asym), Sine Shaper, Stomp Box, Tape Stop"}, {"Dist_PrePost", "Off, Pre, Post"}, {"Dist_Freq", "8 Hz, 13290 Hz"}, {"Dist_BW", "0.1 - 7.6"}, {"Dist_L/B/H", "0% - 100%"}, {"Dist_Drv", "0% - 100%"}, {"Dist_Wet", "0% - 100%"}, {"Flg Enable", "off, on"}, {"Flg_Rate", "0.00 Hz - 20.00 Hz"}, {"Flg_BPM_Sync", "off, on"}, {"Flg_Dep", "0% - 100%"}, {"Flg_Feed", "0% - 100%"}, {"Flg_Stereo", "22 Hz - 200"}, {"Flg_Wet", "0% - 100%"}, {"Phs Enable", "off, on"}, {"Phs_Rate", "0.00 Hz - 20.00 Hz"}, {"Phs_BPM_Sync", "off, on"}, {"Phs_Dpth", "0% - 100%"}, {"Phs_Frq", "20 Hz - 18000 Hz"}, {"Phs_Feed", "0% - 100%"}, {"Phs_Stereo", "0 deg. - 360 deg."}, {"Phs_Wet", "0% - 100%"}, {"Cho Enable", "off, on"}, {"Cho_Rate", "0.00 Hz - 20.00 Hz"}, {"Cho_BPM_Sync", "off, on"}, {"Cho_Dly", "0.0 ms - 20.0 ms"}, {"Cho_Feed", "0% - 95%"}, {"Cho_Filt", "50 Hz - 20000 Hz"}, {"Cho_Wet", "0% - 100%"}, {"Dly Enable", "off, on"}, {"Dly_BPM_Sync", "off, on"}, {"Dly_TimL", "1.00 - 501.00"}, {"Dly_TimR", "1.00 - 501.00"}, {"Dly_Freq", "40 Hz - 18000 Hz"}, {"Dly_Mode", "Normal, Ping-Pong, Tap->Delay"}, {"Dly_Wet", "0% - 100%"}, {"Comp Enable", "off, on"}, {"Cmp_Thr", "0.0 dB - 120.0 dB"}, {"Cmp_Att", "0.1 ms - 1000.0 ms"}, {"Cmp_Rel", "0.1 ms - 999.1 ms"}, {"CmpGain", "0.0 dB - 30.1 dB"}, {"CmpMBnd", "Normal, MultBand"}, {"Comp_Wet", "0 - 100"}} To facilitate accurate parsing and handling of your requests, please provide parameter adjustments in JSON format when possible. This ensures the correct interpretation and application of your specifications for this C++ program. Don't add newline characters. DO NOT RESPOND WITH ANYTHING BUT THE LIST!)";

    // Combine system instruction with user input
    juce::String finalPrompt = systemInstruction + "\n" + userPrompt;

    // Ensure the total length does not exceed OpenAI's limit
    if (finalPrompt.length() > MAX_OPENAI_LENGTH)
    {
        DBG("Prompt too long, truncating user input slightly...");
        finalPrompt = systemInstruction + "\n" + userPrompt.substring(0, MAX_OPENAI_LENGTH - systemInstruction.length());
    }

    // Create JSON payload
    juce::var requestPayload = juce::var(new juce::DynamicObject());
    requestPayload.getDynamicObject()->setProperty("model", "gpt-4");
    requestPayload.getDynamicObject()->setProperty("max_tokens", 2000);

    // Create messages array
    juce::var messages = juce::Array<juce::var>();

    // Add system message
    juce::var systemMsgObj = juce::var(new juce::DynamicObject());
    systemMsgObj.getDynamicObject()->setProperty("role", "system");
    systemMsgObj.getDynamicObject()->setProperty("content", systemInstruction);
    messages.getArray()->add(systemMsgObj);

    // Add user message (entire prompt)
    juce::var userMsgObj = juce::var(new juce::DynamicObject());
    userMsgObj.getDynamicObject()->setProperty("role", "user");
    userMsgObj.getDynamicObject()->setProperty("content", userPrompt);
    messages.getArray()->add(userMsgObj);

    requestPayload.getDynamicObject()->setProperty("messages", messages);

    // Convert JSON to compact format
    juce::String requestBody = juce::JSON::toString(requestPayload, false);
    requestBody = requestBody.replace("\n", "").replace("  ", ""); // Strip newlines & spaces
    DBG("Request body:" + requestBody);
    DBG("Final Request JSON Length: " + juce::String(requestBody.length()));

    // Create URL and attach POST data
    juce::URL url(apiEndpoint);
    url = url.withPOSTData(requestBody);

    // Create headers correctly
    juce::StringArray headers;
    headers.add("Authorization: Bearer " + apiKey);
    headers.add("Content-Type: application/json");
    headers.add("Accept: application/json");

    // Create WebInputStream (POST request)
    std::unique_ptr<juce::WebInputStream> webStream = std::make_unique<juce::WebInputStream>(
        url, true // true = POST request
    );

    // Set headers properly
    webStream->withExtraHeaders(headers.joinIntoString("\r\n"));
    webStream->withConnectionTimeout(CONNECTION_TIMEOUT_MS);

    // Open the connection
    if (!webStream->connect(nullptr))
    {
        if (auto* editor = dynamic_cast<CGPTxSerumAudioProcessorEditor*>(getParentComponent()->getParentComponent()))
        {
            editor->setLoading(false);// Show loading screen
            
        }
        DBG("Error: Failed to connect to API.");
        return std::map<std::string, std::string>();
    }

    // Read response in chunks to avoid buffer limits
    // Read response in chunks to avoid buffer limits
    juce::MemoryOutputStream responseStream;
    const uint32_t startTime = juce::Time::getMillisecondCounter();

    while (!webStream->isExhausted())
    {
        // Check for read timeout
        if (juce::Time::getMillisecondCounter() - startTime > READ_TIMEOUT_MS)
        {
            if (auto* editor = dynamic_cast<CGPTxSerumAudioProcessorEditor*>(getParentComponent()->getParentComponent()))
            {
                editor->setLoading(false);// Show loading screen
                
            }
            DBG("Error: Read timeout exceeded");
            return std::map<std::string, std::string>();
        }

        // Read in larger chunks (32KB) for better performance
        if (responseStream.writeFromInputStream(*webStream, 32768) == 0)
        {
            // If we got 0 bytes, wait a bit before trying again
            juce::Thread::sleep(100);
            continue;
        }
    }

    juce::String response = responseStream.toString();
    DBG("Raw API Response: " + response);

    if (response.isEmpty())
    {
        if (auto* editor = dynamic_cast<CGPTxSerumAudioProcessorEditor*>(getParentComponent()->getParentComponent()))
        {
            editor->setLoading(false);// Show loading screen
            
        }
        return std::map<std::string, std::string>();
    }

    // Parse JSON response
    const juce::var jsonResponse = juce::JSON::parse(response);
    if (!jsonResponse.isObject())
    {
        if (auto* editor = dynamic_cast<CGPTxSerumAudioProcessorEditor*>(getParentComponent()->getParentComponent()))
        {
            editor->setLoading(false);// Show loading screen
            
        }
        return std::map<std::string, std::string>();
    }

    if (auto* obj = jsonResponse.getDynamicObject())
    {
        if (obj->hasProperty("choices"))
        {
            const juce::var choices = obj->getProperty("choices");
            if (choices.isArray() && choices.getArray() != nullptr && choices.getArray()->size() > 0)
            {
                const juce::var firstChoice = choices.getArray()->getFirst();
                if (auto* choiceObj = firstChoice.getDynamicObject())
                {
                    if (choiceObj->hasProperty("message"))
                    {
                        const juce::var msg = choiceObj->getProperty("message");
                        if (auto* msgObj = msg.getDynamicObject())
                        {
                            if (msgObj->hasProperty("content"))
                            {
                                juce::String content = msgObj->getProperty("content").toString();
                                DBG("ChatGPT Response Content: " + content);

                                std::map<std::string, std::string> parameterMap;

                                content = content.replace("\r\n", "\n").replace("\r", "\n"); // Convert to consistent \n
                                DBG("ChatGPT Response Content (Normalized): " + content);

                                if (content.startsWith("[") && content.endsWith("]")) {
                                    content = content.substring(1, content.length() - 1).trim(); // Remove brackets and trim
                                    DBG("Removed surrounding brackets: " + content);
                                }

                                // 1. Split the content into lines:
                                int startPos = 0;
                                while (content.isNotEmpty()) {
                                    if (content.contains("{") && content.contains("}")) {
                                        int openBracePos = content.indexOf("{");
                                        int closeBracePos = content.indexOf("}");

                                        if (openBracePos < closeBracePos) { // Ensure correct order
                                            juce::String pairString = content.substring(openBracePos + 1, closeBracePos).trim();

                                            if (pairString.contains(",")) {
                                                int commaPos = pairString.indexOf(",");
                                                juce::String name = pairString.substring(0, commaPos).trim();
                                                juce::String value = pairString.substring(commaPos + 1).trim();

                                                name = name.removeCharacters("\"");
                                                value = value.removeCharacters("\"");

                                                parameterMap[name.toStdString()] = value.toStdString();
                                            }

                                            // Remove the processed pair from the content string:
                                            content = content.substring(closeBracePos + 1).trim();
                                        }
                                        else {
                                            // Handle cases where braces are in the wrong order
                                            DBG("Warning: Invalid brace order in content: " + content);
                                            break; // Or handle as needed
                                        }
                                    }
                                    else {
                                        // No more complete pairs found, exit loop
                                        break;
                                    }
                                }




                                // Debug output
                                DBG("Created map with " + juce::String(parameterMap.size()) + " parameters");

                                


                                //parameter map stuff

                                juce::String mapContents; // To store the formatted output

                                for (const auto& pair : parameterMap) {
                                    mapContents += "\"" + juce::String(pair.first) + "\", \"" + juce::String(pair.second) + "\"\n"; // Add each pair with a newline
                                }

                                if (!mapContents.isEmpty()) { // Only log if there are parameters
                                    juce::Logger::writeToLog("Parameter Map Contents:\n" + mapContents);
                                }
                                else {
                                    juce::Logger::writeToLog("Parameter Map is empty.");
                                }


                                //loading screen termination
                                if (auto* editor = dynamic_cast<CGPTxSerumAudioProcessorEditor*>(getParentComponent()->getParentComponent()))
                                {
                                    editor->setLoading(false);// Show loading screen
                                    
                                }


                                return parameterMap;
                            }
                        }
                    }
                }
            }
        }
    }
    if (auto* editor = dynamic_cast<CGPTxSerumAudioProcessorEditor*>(getParentComponent()->getParentComponent()))
    {
        editor->setLoading(false);// Show loading screen
        
    }
    return std::map<std::string, std::string>();
}









ChatBarComponent::~ChatBarComponent()
{
    sendButton.setLookAndFeel(nullptr);

}


void ChatBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void ChatBarComponent::resized()
{
    auto area = getLocalBounds();

    auto chatBarHeight = 25;    
    auto chatBarWidth = 600;   
    auto buttonWidth = 100;

    auto yPosition = (getHeight() - chatBarHeight) / 2;

    chatInput.setBounds((getWidth() - chatBarWidth - buttonWidth - 10) / 2, yPosition + -50, chatBarWidth, chatBarHeight);

    sendButton.setBounds(chatInput.getRight() + 10, yPosition -50, buttonWidth, chatBarHeight);

  
}



void ChatBarComponent::sendAIResponseToProcessor(const std::map<std::string, std::string>& aiResponse)
{
    processor.applyPresetToSerum(aiResponse);
}


