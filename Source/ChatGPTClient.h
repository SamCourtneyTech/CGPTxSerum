/*
#pragma once
// ChatGPTClient.h
#pragma once

#include <JuceHeader.h>
#include <string>
#include <vector>
#include <utility>

class ChatGPTClient {
public:
    ChatGPTClient(const std::string& apiKey);
    ~ChatGPTClient();

    // Main function to send prompt and get parameters
    std::vector<std::pair<String, String>> sendPromptToChatGPT(const String& userInput);

private:
    const std::string apiKey;
    const std::string apiEndpoint;

    std::string makeRequest(const std::string& prompt);
    std::vector<std::pair<String, String>> parseResponse(const std::string& response);
};
*/