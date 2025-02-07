// ChatGPTClient.cpp
/*
#include "ChatGPTClient.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Callback function for CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

ChatGPTClient::ChatGPTClient(const std::string& apiKey)
    : apiKey(apiKey)
    , apiEndpoint("https://api.openai.com/v1/chat/completions")
{
    curl_global_init(CURL_GLOBAL_ALL);
}

ChatGPTClient::~ChatGPTClient() {
    curl_global_cleanup();
}

std::string ChatGPTClient::makeRequest(const std::string& prompt) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        // Prepare headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string authHeader = "Authorization: Bearer " + apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());

        // Prepare request body
        json requestBody = {
            {"model", "gpt-4"},
            {"messages", json::array({
                {
                    {"role", "system"},
                    {"content", "You are a synthesizer preset generator. Respond only with a JSON array of parameter-value pairs."}
                },
                {
                    {"role", "user"},
                    {"content", prompt}
                }
            })},
            {"temperature", 0.7}
        };

        std::string requestBodyStr = requestBody.dump();

        // Set up CURL options
        curl_easy_setopt(curl, CURLOPT_URL, apiEndpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform request
        CURLcode res = curl_easy_perform(curl);

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to make API request: " + std::string(curl_easy_strerror(res)));
        }
    }

    return response;
}

std::vector<std::pair<String, String>> ChatGPTClient::parseResponse(const std::string& response) {
    std::vector<std::pair<String, String>> parameters;

    try {
        json responseJson = json::parse(response);
        std::string content = responseJson["choices"][0]["message"]["content"];
        json parameterList = json::parse(content);

        for (const auto& param : parameterList) {
            for (const auto& [key, value] : param.items()) {
                parameters.push_back(std::make_pair(
                    String(key.c_str()),
                    String(value.get<std::string>().c_str())
                ));
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing response: " << e.what() << std::endl;
    }

    return parameters;
}

std::vector<std::pair<String, String>> ChatGPTClient::sendPromptToChatGPT(const String& userInput) {
    try {
        std::string response = makeRequest(userInput.toStdString());
        return parseResponse(response);
    }
    catch (const std::exception& e) {
        std::cerr << "Error in sendPromptToChatGPT: " << e.what() << std::endl;
        return {};
    }
}
*/