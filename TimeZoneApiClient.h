#pragma once
// TimeZoneApiClient.h
// API client for communicating with the TimeZoneDB API
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "TimeZone.h"

using json = nlohmann::json;

// Class to handle API communication with TimeZoneDB
class TimeZoneApiClient {
private:
    std::string apiKey;     // API key for the time zone service
    std::string apiBaseUrl; // Base URL for API requests
    CURL* curl;             // CURL handle for HTTP requests

    // Helper function to perform API requests
    json makeRequest(const std::string& endpoint, const std::string& params = "");

    // Callback function for CURL to write response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

public:
    // Constructor takes API key and optional base URL
    TimeZoneApiClient(const std::string& key,
        const std::string& url = "http://api.timezonedb.com/v2.1");

    // Destructor to clean up CURL resources
    ~TimeZoneApiClient();

    // Get a list of all time zones
    std::vector<TimeZone> getAllTimeZones();

    // Get current time for a specific time zone
    TimeZone getCurrentTime(const std::string& zone);

    // Convert time between time zones
    bool convertTime(const std::string& fromZone, const std::string& toZone,
        int year, int month, int day, int hour, int minute,
        TimeZone& fromResult, TimeZone& toResult);

    // Search for time zones by region/country
    std::vector<TimeZone> searchTimeZones(const std::string& query);
};
