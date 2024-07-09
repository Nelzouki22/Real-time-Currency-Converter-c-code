#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h> // Include the JSON library header

// Function to write the response data from cURL to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

double getExchangeRate(const std::string& apiKey, const std::string& fromCurrency, const std::string& toCurrency) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    std::string url = "https://v6.exchangerate-api.com/v6/" + apiKey + "/latest/" + fromCurrency;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
            return -1;
        }

        Json::Reader reader;
        Json::Value jsonData;
        if (!reader.parse(readBuffer, jsonData)) {
            std::cerr << "Failed to parse JSON data" << std::endl;
            return -1;
        }

        if (jsonData["result"] != "success") {
            std::cerr << "Error: " << jsonData["error-type"].asString() << std::endl;
            return -1;
        }

        return jsonData["conversion_rates"][toCurrency].asDouble();
    }
    return -1;
}

int main() {
    std::string apiKey = "your_api_key_here";  // Replace with your actual API key
    std::string fromCurrency;
    std::string toCurrency;
    double amount;

    std::cout << "Enter the currency you want to convert from (e.g., USD): ";
    std::cin >> fromCurrency;
    std::cout << "Enter the currency you want to convert to (e.g., EUR): ";
    std::cin >> toCurrency;
    std::cout << "Enter the amount you want to convert: ";
    std::cin >> amount;

    double exchangeRate = getExchangeRate(apiKey, fromCurrency, toCurrency);
    if (exchangeRate != -1) {
        double convertedAmount = amount * exchangeRate;
        std::cout << amount << " " << fromCurrency << " is equal to " << convertedAmount << " " << toCurrency << std::endl;
    }
    else {
        std::cerr << "Failed to get the exchange rate." << std::endl;
    }

    return 0;
}
