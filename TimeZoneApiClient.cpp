// TimeZoneApiClient.cpp
// Implementation of the TimeZoneDB API client

#include "TimeZoneApiClient.h"
#include <iostream>
#include <algorithm>
#include <fstream>

// Static callback function for CURL to write response data
size_t TimeZoneApiClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

TimeZoneApiClient::TimeZoneApiClient(const std::string& key, const std::string& url)
    : apiKey(key), apiBaseUrl(url), curl(nullptr) {

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

TimeZoneApiClient::~TimeZoneApiClient() {
    // Clean up CURL resources
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

json TimeZoneApiClient::makeRequest(const std::string& endpoint, const std::string& params) {
    if (!curl) {
        throw std::runtime_error("CURL not initialized");
    }

    // Construct the full URL with API key
    std::string url = apiBaseUrl + endpoint + "?key=" + apiKey + "&format=json";
    if (!params.empty()) {
        url += "&" + params;
    }

    // Set up CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // Response data will be stored here
    std::string responseData;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }

    // Parse the JSON response
    try {
        json response = json::parse(responseData);

        // Check for API errors
        if (response.contains("status") && response["status"] == "FAILED") {
            std::string message = "API Error";
            if (response.contains("message")) {
                message = response["message"];
            }
            throw std::runtime_error(message);
        }

        return response;
    }
    catch (const json::parse_error& e) {
        throw std::runtime_error("Failed to parse API response: " + std::string(e.what()));
    }
}

std::vector<TimeZone> TimeZoneApiClient::getAllTimeZones() {
    std::vector<TimeZone> timeZones;

    try {
        // Call the list-time-zone endpoint
        json response = makeRequest("/list-time-zone");

        // Parse the response
        if (response.contains("zones") && response["zones"].is_array()) {
            for (const auto& zone : response["zones"]) {
                TimeZone tz;
                tz.code = zone.value("zoneName", "");

                // In TimeZoneDB, countryName + countryCode gives us the region info
                tz.region = zone.value("countryName", "");
                if (zone.contains("countryCode")) {
                    tz.region += " (" + zone["countryCode"].get<std::string>() + ")";
                }

                // Extract the zone name from the code for display
                size_t slashPos = tz.code.find_last_of('/');
                if (slashPos != std::string::npos) {
                    tz.name = tz.code.substr(slashPos + 1);
                    // Replace underscores with spaces
                    std::replace(tz.name.begin(), tz.name.end(), '_', ' ');
                }
                else {
                    tz.name = tz.code;
                }

                // Get the GMT offset (convert from seconds to hours)
                tz.offset = zone.value("gmtOffset", 0) / 3600.0;

                // DST information
                tz.hasDST = (zone.value("dst", "0") == "1");
                tz.dstActive = false; // We'll set this when getting current time

                timeZones.push_back(tz);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting time zones: " << e.what() << std::endl;
    }

    return timeZones;
}

// In TimeZoneApiClient.cpp - getCurrentTime method
TimeZone TimeZoneApiClient::getCurrentTime(const std::string& zone) {
    TimeZone result;
    result.code = zone;

    try {
        // Try the API call first
        std::string params = "zone=" + zone + "&by=zone";
        json response = makeRequest("/get-time-zone", params);

        // Parse the API response (only if we get here without exception)
        if (response.contains("zoneName")) {
            result.code = response["zoneName"];

            // Extract the zone name from the code for display
            size_t slashPos = result.code.find_last_of('/');
            if (slashPos != std::string::npos) {
                result.name = result.code.substr(slashPos + 1);
                // Replace underscores with spaces
                std::replace(result.name.begin(), result.name.end(), '_', ' ');
            }
            else {
                result.name = result.code;
            }

            // Add country/region information if available
            if (response.contains("countryName")) {
                result.region = response["countryName"];
                if (response.contains("countryCode")) {
                    result.region += " (" + response["countryCode"].get<std::string>() + ")";
                }
            }

            // Get the GMT offset (convert from seconds to hours)
            result.offset = response.value("gmtOffset", 0) / 3600.0;

            // DST information
            result.hasDST = response.value("dst", "0") == "1";
            result.dstActive = response.value("dstActive", "0") == "1";

            // Store the formatted time and timestamp directly from the API
            if (response.contains("formatted")) {
                result.formattedTime = response["formatted"];
            }
            if (response.contains("timestamp")) {
                result.timestamp = response["timestamp"];
            }

            return result; // Return successful API result
        }
    }
    catch (const std::exception& e) {
        // API request failed - continue to fallback
        std::cerr << "API error for " << zone << ", using local data instead" << std::endl;
    }

    // Set hardcoded values but also need to set formatted time for fallbacks
    if (zone == "UTC") {
        result.name = "UTC";
        result.region = "Coordinated Universal Time";
        result.offset = 0.0;
        result.hasDST = false;
        result.dstActive = false;

        // Generate formatted time for UTC
        time_t now = time(nullptr);
        struct tm timeinfo;
        gmtime_s(&timeinfo, &now);
        char buffer[30];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        result.formattedTime = buffer;
        result.timestamp = now;
    }
    else if (zone == "America/New_York") {
        // Similar hardcoded fallbacks for other zones...
        result.name = "New York";
        result.region = "United States (US)";
        result.offset = -5.0;
        result.hasDST = true;
        result.dstActive = false;

        // Generate a formatted time for this zone based on offset
        time_t now = time(nullptr);
        time_t localNow = now + static_cast<time_t>(result.offset * 3600);
        struct tm timeinfo;
        gmtime_s(&timeinfo, &localNow);
        char buffer[30];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        result.formattedTime = buffer;
        result.timestamp = now;
    }
    // Continue with other hardcoded fallbacks...

    return result;
}

bool TimeZoneApiClient::convertTime(const std::string& fromZone, const std::string& toZone,
    int year, int month, int day, int hour, int minute,
    TimeZone& fromResult, TimeZone& toResult) {
    try {
        // First, get information about the source time zone
        fromResult = getCurrentTime(fromZone);

        // Create a timestamp for the specified local time in the source zone
        struct tm timeinfo = {};
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = minute;
        timeinfo.tm_sec = 0;

        // Convert to timestamp, adjusting for the source zone's offset
        time_t timestamp = mktime(&timeinfo);
        timestamp -= static_cast<time_t>(fromResult.offset * 3600); // Adjust for zone offset

        if (timestamp == -1) {
            return false;
        }

        // Now get the target time zone information
        toResult = getCurrentTime(toZone);

        // Adjust the timestamp for the target zone's offset
        timestamp += static_cast<time_t>(toResult.offset * 3600);

        // Convert back to human-readable time
        struct tm timeinfo_result_obj;
        gmtime_s(&timeinfo_result_obj, &timestamp);
        struct tm* timeinfo_result = &timeinfo_result_obj;


        if (!timeinfo_result) {
            return false;
        }

        // Update the time info in the target result
        timeinfo = *timeinfo_result;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error converting time: " << e.what() << std::endl;
        return false;
    }
}

std::vector<TimeZone> TimeZoneApiClient::searchTimeZones(const std::string& query) {
    // Get all time zones and filter them client-side
    // This is simpler than trying to use the API's limited search capabilities
    std::vector<TimeZone> allZones = getAllTimeZones();
    std::vector<TimeZone> results;

    // Convert query to lowercase for case-insensitive search
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
        [](unsigned char c) { return std::tolower(c); });

    // Search in both the zone name and region
    for (const auto& zone : allZones) {
        std::string lowerCode = zone.code;
        std::transform(lowerCode.begin(), lowerCode.end(), lowerCode.begin(),
            [](unsigned char c) { return std::tolower(c); });

        std::string lowerRegion = zone.region;
        std::transform(lowerRegion.begin(), lowerRegion.end(), lowerRegion.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (lowerCode.find(lowerQuery) != std::string::npos ||
            lowerRegion.find(lowerQuery) != std::string::npos) {
            results.push_back(zone);
        }
    }

    return results;
}