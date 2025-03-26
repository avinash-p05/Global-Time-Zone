// TimeZoneApp.cpp
// Implementation of the main application class

#include "TimeZoneApp.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <thread>
#include <chrono>
#include <limits> 

TimeZoneApp::TimeZoneApp(const std::string& apiKey, const std::string& configPath)
    : apiClient(apiKey), configFilePath(configPath) {
}

bool TimeZoneApp::initialize() {
    // Load user configuration (favorites)
    return loadUserConfig();
}

bool TimeZoneApp::loadUserConfig() {
    try {
        // Try to open the config file
        std::ifstream file(configFilePath);
        if (!file.is_open()) {
            // No config file yet - that's OK
            return true;
        }

        // Parse the JSON data
        json config;
        file >> config;

        // Load favorites
        if (config.contains("favorites") && config["favorites"].is_array()) {
            favorites.clear();
            for (const auto& item : config["favorites"]) {
                if (item.is_string()) {
                    favorites.push_back(item);
                }
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading user configuration: " << e.what() << std::endl;
        // Non-fatal error - continue with defaults
        return true;
    }
}

bool TimeZoneApp::saveUserConfig() {
    try {
        json config;

        // Save favorites
        config["favorites"] = favorites;

        // Write to file
        std::ofstream file(configFilePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open config file for writing" << std::endl;
            return false;
        }

        file << std::setw(4) << config << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving user configuration: " << e.what() << std::endl;
        return false;
    }
}

void TimeZoneApp::displayUTCTime() {
    try {
        // Get current UTC time using the API
        TimeZone utc = apiClient.getCurrentTime("UTC");

        // Display time directly from the API response
        std::cout << "Current UTC Time: " << utc.formattedTime << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error displaying UTC time: " << e.what() << std::endl;
    }
    waitForKeypress();
}

void TimeZoneApp::displayAllTimeZones() {
    try {
        // Get all time zones from the API
        auto timeZones = apiClient.getAllTimeZones();

        //// To print the size of the vector
        //std::cout << "Get All time zones response - found " << timeZones.size() << " time zones" << std::endl;

        //// Or to print details of each time zone (limit to first few to avoid overwhelming output)
        //std::cout << "Get All time zones response - sample of first 5 zones:" << std::endl;
        /*int count = 0;
        for (const auto& tz : timeZones) {
            if (count++ >= 10) break;
            std::cout << "  - " << tz.code << " (" << tz.region << ")" << std::endl;
        }*/

        std::cout << "Current time in all time zones:" << std::endl;
        std::cout << std::string(95, '-') << std::endl;
        std::cout << std::left << std::setw(30) << "Time Zone" << " | "
            << std::left << std::setw(20) << "Region" << " | "
            << std::left << std::setw(12) << "UTC Offset" << " | "
            << std::left << std::setw(20) << "Current Time" << " | "
            << "DST Status" << std::endl;

        std::cout << std::string(95, '-') << std::endl;

        // Limit the display to a reasonable number
        const int maxDisplay = 20;
        int displayed = 0;

        for (const auto& tz : timeZones) {
            // Check if this is a favorite
            bool isFavorite = std::find(favorites.begin(), favorites.end(), tz.code) != favorites.end();

            // Only display major time zones and favorites to avoid overwhelming output
            if (isFavorite ||
                tz.code == "UTC" ||
                tz.code.find("America/New_York") != std::string::npos ||
                tz.code.find("America/Los_Angeles") != std::string::npos ||
                tz.code.find("Europe/London") != std::string::npos ||
                tz.code.find("Europe/Paris") != std::string::npos ||
                tz.code.find("Asia/Tokyo") != std::string::npos ||
                tz.code.find("Asia/Kolkata") != std::string::npos) {

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                // Get current time for this zone
                TimeZone currentTime = apiClient.getCurrentTime(tz.code);

                // Use the formatted time directly from the API
                std::string timeStr = currentTime.formattedTime;

                // Format offset string
                std::stringstream offsetStr;
                offsetStr << (currentTime.offset >= 0 ? "+" : "") << currentTime.offset;

                // Star for favorites
                std::string displayName = currentTime.code + (isFavorite ? " *" : "");

                std::cout << std::left << std::setw(30) << displayName << " | "
                    << std::left << std::setw(20) << currentTime.region << " | "
                    << std::left << std::setw(12) << offsetStr.str() << " | "
                    << std::left << std::setw(20) << timeStr << " | "
                    << (currentTime.dstActive ? "DST active" : (currentTime.hasDST ? "Standard Time" : "No DST"))
                    << std::endl;

                displayed++;
                if (displayed >= maxDisplay) {
                    break;
                }
            }
        }

        std::cout << std::endl;
        std::cout << "Note: Displaying major time zones and favorites only. Use search to find others." << std::endl;
        std::cout << "Time zones marked with * are in your favorites." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error displaying time zones: " << e.what() << std::endl;
    }
    waitForKeypress();
}

void TimeZoneApp::displayTimeZone(const std::string& code) {
    try {
        // Get current time for this zone
        TimeZone tz = apiClient.getCurrentTime(code);

        if (tz.code.empty()) {
            std::cout << "Time zone not found." << std::endl;
            return;
        }

        // Format offset string
        std::stringstream offsetStr;
        offsetStr << (tz.offset >= 0 ? "+" : "") << tz.offset;

        // Display time zone information
        std::cout << tz.code << " (" << tz.name << ")" << std::endl;
        std::cout << "Region: " << tz.region << std::endl;
        std::cout << "Current Time: " << tz.formattedTime << std::endl;
        std::cout << "UTC Offset: " << offsetStr.str() << " hours"
            << (tz.dstActive ? " (includes DST)" : "") << std::endl;
        std::cout << "DST Status: "
            << (tz.dstActive ? "Active" : (tz.hasDST ? "Inactive" : "Not observed"))
            << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error displaying time zone: " << e.what() << std::endl;
    }
    waitForKeypress();
}

void TimeZoneApp::convertTime() {
    // Get source time zone
    std::string fromZone;
    std::cout << "Enter source time zone (e.g., America/New_York): ";
    std::getline(std::cin, fromZone);
    // Get target time zone
    std::string toZone;
    std::cout << "Enter target time zone (e.g., Europe/London): ";
    std::getline(std::cin, toZone);
    // Get date and time to convert
    int year, month, day, hour, minute;
    try {
        std::cout << "Enter date (YYYY MM DD): ";
        std::cin >> year >> month >> day;
        if (std::cin.fail() || year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
            throw std::invalid_argument("Invalid date values");
        }
        std::cout << "Enter time (HH MM): ";
        std::cin >> hour >> minute;
        if (std::cin.fail() || hour < 0 || hour > 23 || minute < 0 || minute > 59) {
            throw std::invalid_argument("Invalid time values");
        }
        std::cin.ignore(); // Clear input buffer
        // Perform the conversion
        TimeZone fromResult, toResult;
        if (apiClient.convertTime(fromZone, toZone, year, month, day, hour, minute,
            fromResult, toResult)) {
            // Format the source time
            char fromTimeStr[100];
            sprintf_s(fromTimeStr, sizeof(fromTimeStr), "%04d-%02d-%02d %02d:%02d", year, month, day, hour, minute);

            // Display results
            std::cout << std::endl;
            std::cout << fromTimeStr << " in " << fromResult.code;
            if (fromResult.dstActive) std::cout << " (DST active)";
            std::cout << std::endl;
            std::cout << "is equivalent to:" << std::endl;
            std::cout << toResult.formattedTime << " in " << toResult.code;
            if (toResult.dstActive) std::cout << " (DST active)";
            std::cout << std::endl;
        }
        else {
            std::cout << "Time conversion failed. Please check your input." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cin.clear();  // Clear error flags
        std::cin.ignore(10000, '\n');  // Clear input buffer
        std::cout << "Invalid input: " << e.what() << std::endl;
    }
    waitForKeypress();
}

void TimeZoneApp::displayFavorites() {
    if (favorites.empty()) {
        std::cout << "You have no favorite time zones set." << std::endl;
        std::cout << "Use option 5 to add favorites." << std::endl;
        return;
    }

    std::cout << "Current time in your favorite time zones:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    for (const auto& tzCode : favorites) {
        try {
            // Get current time for this zone
            TimeZone tz = apiClient.getCurrentTime(tzCode);

            if (tz.code.empty()) {
                std::cout << tzCode << " - Time zone not found" << std::endl;
                continue;
            }

            // Format offset string
            std::stringstream offsetStr;
            offsetStr << (tz.offset >= 0 ? "+" : "") << tz.offset;

            // Display time zone information
            std::cout << tz.code << " (" << tz.name << ")" << std::endl;
            std::cout << "  Region: " << tz.region << std::endl;
            std::cout << "  Current Time: " << tz.formattedTime << std::endl;
            std::cout << "  UTC Offset: " << offsetStr.str() << " hours"
                << (tz.dstActive ? " (includes DST)" : "") << std::endl;
            std::cout << "  DST Status: "
                << (tz.dstActive ? "Active" : (tz.hasDST ? "Inactive" : "Not observed"))
                << std::endl;
            std::cout << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << tzCode << " - Error: " << e.what() << std::endl;
            std::cout << std::endl;
        }
    }
    waitForKeypress();
}

void TimeZoneApp::manageFavorites() {
    std::string tzCode;
    std::cout << "Enter time zone to add/remove from favorites: ";
    std::getline(std::cin, tzCode);

    // Verify the time zone exists
    try {
        TimeZone tz = apiClient.getCurrentTime(tzCode);

        if (tz.code.empty()) {
            std::cout << "Time zone not found." << std::endl;
            return;
        }

        // Check if already in favorites
        auto it = std::find(favorites.begin(), favorites.end(), tzCode);
        if (it != favorites.end()) {
            // Remove from favorites
            favorites.erase(it);
            std::cout << tzCode << " removed from favorites." << std::endl;
        }
        else {
            // Add to favorites
            favorites.push_back(tzCode);
            std::cout << tzCode << " added to favorites." << std::endl;
        }

        // Save the updated favorites
        saveUserConfig();
    }
    catch (const std::exception& e) {
        std::cerr << "Error managing favorites: " << e.what() << std::endl;
    }
    waitForKeypress();
}

void TimeZoneApp::searchTimeZones() {
    std::string query;
    std::cout << "Enter city, region or country to search: ";
    std::getline(std::cin, query);

    if (query.empty()) {
        std::cout << "Search term cannot be empty." << std::endl;
        return;
    }

    try {
        // Search for matching time zones
        auto results = apiClient.searchTimeZones(query);

        if (results.empty()) {
            std::cout << "No time zones found matching '" << query << "'." << std::endl;
            return;
        }

        std::cout << "Found " << results.size() << " matching time zones:" << std::endl;
        std::cout << std::string(70, '-') << std::endl;

        // Limit results to avoid overwhelming display
        const int maxResults = 15;
        int displayed = 0;

        for (const auto& tz : results) {
            // Check if this is a favorite
            bool isFavorite = std::find(favorites.begin(), favorites.end(), tz.code) != favorites.end();

            // Star for favorites
            std::string displayName = tz.code + (isFavorite ? " *" : "");

            // Format offset string
            std::stringstream offsetStr;
            offsetStr << (tz.offset >= 0 ? "+" : "") << tz.offset;

            std::cout << displayName << std::endl;
            std::cout << "  Region: " << tz.region << std::endl;
            std::cout << "  UTC Offset: " << offsetStr.str() << " hours" << std::endl;

            if (tz.hasDST) {
                std::cout << "  Observes DST: Yes" << std::endl;
            }

            std::cout << std::endl;

            displayed++;
            if (displayed >= maxResults) {
                std::cout << "Showing first " << maxResults << " results. Refine your search for more specific results." << std::endl;
                break;
            }
        }

        if (std::any_of(results.begin(), results.end(), [&](const TimeZone& tz) {
            return std::find(favorites.begin(), favorites.end(), tz.code) != favorites.end();
            })) {
            std::cout << "Note: Time zones marked with * are in your favorites." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error searching time zones: " << e.what() << std::endl;
    }
    waitForKeypress();
}

void TimeZoneApp::displayHelp() {
    std::cout << "GLOBAL TIME ZONE APPLICATION - HELP" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;

    std::cout << "TIME ZONE CONCEPTS:" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "UTC (Coordinated Universal Time): The primary time standard by which the" << std::endl;
    std::cout << "    world regulates clocks and time. It does not observe daylight saving time." << std::endl;
    std::cout << std::endl;

    std::cout << "Time Zone Names: This application uses IANA time zone database names," << std::endl;
    std::cout << "    such as 'America/New_York' or 'Europe/London'. These names provide accurate" << std::endl;
    std::cout << "    location-based time information including DST rules." << std::endl;
    std::cout << std::endl;

    std::cout << "Daylight Saving Time (DST): The practice of setting clocks forward by one hour" << std::endl;
    std::cout << "    during summer months to make better use of daylight. Not all regions observe DST." << std::endl;
    std::cout << std::endl;

    std::cout << "APPLICATION FEATURES:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "1. View current UTC time - Displays the current time in UTC" << std::endl;
    std::cout << "2. View current time in major time zones - Shows time in common zones" << std::endl;
    std::cout << "3. View current time in a specific time zone - Shows detailed info for one zone" << std::endl;
    std::cout << "4. Convert time between time zones - Converts a specific date/time between zones" << std::endl;
    std::cout << "5. Manage favorite time zones - Add or remove time zones from your favorites" << std::endl;
    std::cout << "6. View favorite time zones - Display current time in your favorite zones" << std::endl;
    std::cout << "7. Search time zones - Find time zones by city, region, or country name" << std::endl;
    std::cout << "8. Help - Display this help information" << std::endl;
    std::cout << "9. Exit - Exit the application" << std::endl;
    std::cout << std::endl;

    std::cout << "TIME ZONE NAME EXAMPLES:" << std::endl;
    std::cout << "-----------------------" << std::endl;
    std::cout << "UTC                      - Coordinated Universal Time" << std::endl;
    std::cout << "America/New_York         - Eastern Time (US)" << std::endl;
    std::cout << "America/Chicago          - Central Time (US)" << std::endl;
    std::cout << "America/Denver           - Mountain Time (US)" << std::endl;
    std::cout << "America/Los_Angeles      - Pacific Time (US)" << std::endl;
    std::cout << "Europe/London            - United Kingdom" << std::endl;
    std::cout << "Europe/Paris             - France" << std::endl;
    std::cout << "Europe/Berlin            - Germany" << std::endl;
    std::cout << "Asia/Tokyo               - Japan" << std::endl;
    std::cout << "Asia/Shanghai            - China" << std::endl;
    std::cout << "Australia/Sydney         - Eastern Australia" << std::endl;
    std::cout << "Pacific/Auckland         - New Zealand" << std::endl;
    std::cout << std::endl;

    std::cout << "Use the search feature to find more time zones." << std::endl;

    waitForKeypress();
}

void TimeZoneApp::run() {
    int choice = 0;

    do {
        std::cout << "\nGlobal Time Zone Application" << std::endl;
        std::cout << "===========================" << std::endl;
        std::cout << "1. View current UTC time" << std::endl;
        std::cout << "2. View current time in major time zones" << std::endl;
        std::cout << "3. View current time in a specific time zone" << std::endl;
        std::cout << "4. Convert time between time zones" << std::endl;
        std::cout << "5. Manage favorite time zones" << std::endl;
        std::cout << "6. View favorite time zones" << std::endl;
        std::cout << "7. Search time zones" << std::endl;
        std::cout << "8. Help" << std::endl;
        std::cout << "9. Exit" << std::endl;
        std::cout << "Enter choice: ";

        try {
            std::cin >> choice;
            if (std::cin.fail()) {
                throw std::invalid_argument("Invalid input");
            }
            std::cin.ignore(); // Clear the newline
        }
        catch (const std::exception&) {
            std::cin.clear();  // Clear error flags
            std::cin.ignore(10000, '\n');  // Clear input buffer
            choice = 0;  // Invalid choice
        }

        switch (choice) {
        case 1:
            displayUTCTime();
            break;

        case 2:
            displayAllTimeZones();
            break;

        case 3: {
            std::string tzName;
            std::cout << "Enter time zone (e.g., America/New_York): ";
            std::getline(std::cin, tzName);
            displayTimeZone(tzName);
            break;
        }

        case 4:
            convertTime();
            break;

        case 5:
            manageFavorites();
            break;

        case 6:
            displayFavorites();
            break;

        case 7:
            searchTimeZones();
            break;

        case 8:
            displayHelp();
            break;

        case 9:
            std::cout << "Exiting application. Goodbye!" << std::endl;
            break;

        default:
            std::cout << "Invalid choice. Please enter a number between 1 and 9." << std::endl;
        }

    } while (choice != 9);
}

void TimeZoneApp::waitForKeypress() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(9223372036854775807, '\n');
}

//9223372036854775807: This is the maximum value for std::streamsize on a 64-bit system.
// By using this value directly, you ensure that the 
//ignore function will skip all characters until the newline character is encountered.
