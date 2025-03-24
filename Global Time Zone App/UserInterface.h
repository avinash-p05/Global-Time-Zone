#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include "TimeZoneManager.h"
#include "DateTime.h"

class UserInterface {
private:
    TimeZoneManager& tzManager;

public:
    UserInterface(TimeZoneManager& manager) : tzManager(manager) {}

    // Display the main menu
    void displayMainMenu();

    // Display current time in various time zones
    void viewCurrentTime();

    // Convert time between time zones
    void convertTime();

    // Manage favorite time zones
    void manageFavorites();

    // Display help information
    void showHelp();

    // Display all available time zones
    void displayTimeZones();

    // Display a formatted time
    void displayTime(const DateTime& time);

    // Clear the console screen
    void clearScreen();

    // Wait for a keypress
    void waitForKeypress();
};

#endif // USERINTERFACE_H