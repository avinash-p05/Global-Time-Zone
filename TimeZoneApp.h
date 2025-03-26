#pragma once
// TimeZoneApp.h
// Main application class for the time zone application
#include <string>
#include <vector>
#include "TimeZoneApiClient.h"

// Application class that uses the API client
class TimeZoneApp {
private:
    TimeZoneApiClient apiClient;        // API client for time zone data
    std::vector<std::string> favorites; // Favorite time zone codes
    std::string configFilePath;         // Path to user configuration file

    // User interface methods
    void displayUTCTime();
    void displayAllTimeZones();
    void displayTimeZone(const std::string& code);
    void convertTime();
    void manageFavorites();
    void displayFavorites();
    void searchTimeZones();
    void displayHelp();

    // Config file methods
    bool loadUserConfig();
    bool saveUserConfig();

public:
    // Constructor takes API key and config file path
    TimeZoneApp(const std::string& apiKey, const std::string& configPath = "user_config.json");

    // Initialize the application
    bool initialize();

    // Run the main application loop
    void run();
};