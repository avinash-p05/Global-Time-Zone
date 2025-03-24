#include "UserInterface.h"

// Display the main menu
void UserInterface::displayMainMenu() {
    clearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "   GLOBAL TIME ZONE APPLICATION" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. View Current Time in Different Zones" << std::endl;
    std::cout << "2. Convert Time Between Time Zones" << std::endl;
    std::cout << "3. Manage Favorite Time Zones" << std::endl;
    std::cout << "4. Toggle Daylight Savings Time (Currently "
        << (tzManager.isDSTEnabled() ? "ON" : "OFF") << ")" << std::endl;
    std::cout << "5. Help Information" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "========================================" << std::endl;
}

// Display current time in various time zones
void UserInterface::viewCurrentTime() {
    std::cout << "Current Time Around the World" << std::endl;
    std::cout << "=============================" << std::endl;

    // First show favorites if any
    const auto& favorites = tzManager.getFavoriteTimeZones();
    if (!favorites.empty()) {
        std::cout << "\nFavorite Time Zones:" << std::endl;
        std::cout << "-----------------" << std::endl;
        for (const auto& zone : favorites) {
            if (TimeZone* tz = tzManager.findTimeZone(zone)) {
                DateTime currentTime = tzManager.getCurrentTime(zone);
                std::cout << std::left << std::setw(15) << zone << ": ";
                displayTime(currentTime);
            }
        }
        std::cout << std::endl;
    }

    std::cout << "\nAll Time Zones:" << std::endl;
    std::cout << "------------" << std::endl;
    for (const auto& tz : tzManager.getAllTimeZones()) {
        // Only show main time zones, not cities
        if (tz.region != "USA" && tz.region != "UK" &&
            tz.region != "France" && tz.region != "Germany" &&
            tz.region != "India" && tz.region != "Japan" &&
            tz.region != "Australia") {
            DateTime currentTime = tzManager.getCurrentTime(tz.name);
            std::cout << std::left << std::setw(15) << tz.name << ": ";
            displayTime(currentTime);
        }
    }

    waitForKeypress();
}

// Convert time between time zones
void UserInterface::convertTime() {
    std::string sourceZone, targetZone;
    int year, month, day, hour, minute;

    // Display available time zones
    displayTimeZones();

    std::cout << "\nEnter source time zone: ";
    std::getline(std::cin, sourceZone);

    TimeZone* sourceTZ = tzManager.findTimeZone(sourceZone);
    if (!sourceTZ) {
        std::cout << "Error: Time zone '" << sourceZone << "' not found." << std::endl;
        waitForKeypress();
        return;
    }

    std::cout << "Enter target time zone: ";
    std::getline(std::cin, targetZone);

    TimeZone* targetTZ = tzManager.findTimeZone(targetZone);
    if (!targetTZ) {
        std::cout << "Error: Time zone '" << targetZone << "' not found." << std::endl;
        waitForKeypress();
        return;
    }

    std::cout << "\nEnter date and time for " << sourceZone << ":" << std::endl;
    std::cout << "Year (e.g., 2025): ";
    std::cin >> year;
    std::cout << "Month (1-12): ";
    std::cin >> month;
    std::cout << "Day (1-31): ";
    std::cin >> day;
    std::cout << "Hour (0-23): ";
    std::cin >> hour;
    std::cout << "Minute (0-59): ";
    std::cin >> minute;

    // Validate input
    if (month < 1 || month > 12 || day < 1 || day > 31 ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        std::cout << "Error: Invalid date or time values." << std::endl;
        waitForKeypress();
        return;
    }

    // Create source DateTime
    DateTime sourceTime = {
        year, month, day, hour, minute,
        sourceZone,
        tzManager.isDSTEnabled() && tzManager.isDSTActive(*sourceTZ, month, day)
    };

    // Convert to target time zone
    DateTime targetTime = tzManager.convertTimeZone(sourceTime, targetZone);

    // Display result
    std::cout << "\nConversion Result:" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << sourceZone << ": ";
    displayTime(sourceTime);
    std::cout << targetZone << ": ";
    displayTime(targetTime);

    waitForKeypress();
}

// Manage favorite time zones
void UserInterface::manageFavorites() {
    int choice;
    std::string zoneName;

    while (true) {
        clearScreen();
        std::cout << "Manage Favorite Time Zones" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << "Current Favorites:" << std::endl;

        const auto& favorites = tzManager.getFavoriteTimeZones();
        if (favorites.empty()) {
            std::cout << "  (None)" << std::endl;
        }
        else {
            for (size_t i = 0; i < favorites.size(); i++) {
                std::cout << "  " << i + 1 << ". " << favorites[i] << std::endl;
            }
        }

        std::cout << "\n1. Add a favorite time zone" << std::endl;
        std::cout << "2. Remove a favorite time zone" << std::endl;
        std::cout << "3. Return to main menu" << std::endl;
        std::cout << "\nEnter your choice: ";

        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            displayTimeZones();
            std::cout << "\nEnter time zone to add as favorite: ";
            std::getline(std::cin, zoneName);

            if (tzManager.addFavorite(zoneName)) {
                std::cout << zoneName << " added to favorites." << std::endl;
            }
            else {
                if (tzManager.findTimeZone(zoneName)) {
                    std::cout << zoneName << " is already in favorites." << std::endl;
                }
                else {
                    std::cout << "Error: Time zone '" << zoneName << "' not found." << std::endl;
                }
            }
            waitForKeypress();
            break;

        case 2:
            if (tzManager.getFavoriteTimeZones().empty()) {
                std::cout << "No favorites to remove." << std::endl;
                waitForKeypress();
                break;
            }

            std::cout << "Enter the number of the favorite to remove: ";
            int index;
            std::cin >> index;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (tzManager.removeFavorite(index - 1)) {
                std::cout << "Time zone removed from favorites." << std::endl;
            }
            else {
                std::cout << "Invalid number." << std::endl;
            }
            waitForKeypress();
            break;

        case 3:
            return;

        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            waitForKeypress();
        }
    }
}

// Display help information
void UserInterface::showHelp() {
    std::cout << "Global Time Zone Application - Help Information" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "\nTime Zone Basics:" << std::endl;
    std::cout << "- Time zones are regions that observe the same standard time." << std::endl;
    std::cout << "- They are typically expressed as an offset from Coordinated Universal Time (UTC)." << std::endl;
    std::cout << "- For example, Eastern Standard Time (EST) is UTC-5, meaning it is 5 hours behind UTC." << std::endl;

    std::cout << "\nDaylight Saving Time (DST):" << std::endl;
    std::cout << "- Many regions adjust their clocks forward by one hour during summer months." << std::endl;
    std::cout << "- This is known as Daylight Saving Time." << std::endl;
    std::cout << "- The start and end dates for DST vary by region." << std::endl;
    std::cout << "- When DST is active, time zones are typically referred to with 'D' instead of 'S'." << std::endl;
    std::cout << "  For example, EDT (Eastern Daylight Time) instead of EST (Eastern Standard Time)." << std::endl;

    std::cout << "\nUsing This Application:" << std::endl;
    std::cout << "- View Current Time: Shows the current time in various time zones." << std::endl;
    std::cout << "- Convert Time: Allows you to convert a specific time from one zone to another." << std::endl;
    std::cout << "- Manage Favorites: Add frequently used time zones for quick access." << std::endl;
    std::cout << "- Toggle DST: Turn DST adjustments on or off globally." << std::endl;

    std::cout << "\nNote: This application uses simplified DST rules and may not account" << std::endl;
    std::cout << "for all regional variations or historical changes in time zone policies." << std::endl;

    waitForKeypress();
}

// Display all available time zones
void UserInterface::displayTimeZones() {
    std::cout << "Available Time Zones:" << std::endl;
    std::cout << "====================" << std::endl;

    for (const auto& tz : tzManager.getAllTimeZones()) {
        int hours = tz.utcOffset / 60;
        int minutes = abs(tz.utcOffset % 60);
        std::string sign = (tz.utcOffset >= 0) ? "+" : "-";

        std::cout << std::left << std::setw(15) << tz.name
            << "UTC" << sign << abs(hours) << ":" << std::setfill('0') << std::setw(2) << minutes
            << std::setfill(' ') << " (" << tz.region << ")"
            << (tz.hasDST ? " [Has DST]" : "") << std::endl;
    }
}

// Display a formatted time
void UserInterface::displayTime(const DateTime& time) {
    std::string dstIndicator = time.isDST ? " (DST)" : "";

    // Format: YYYY-MM-DD HH:MM
    std::cout << time.year << "-"
        << std::setfill('0') << std::setw(2) << time.month << "-"
        << std::setfill('0') << std::setw(2) << time.day << " "
        << std::setfill('0') << std::setw(2) << time.hour << ":"
        << std::setfill('0') << std::setw(2) << time.minute
        << dstIndicator << std::endl;
}

// Clear the console screen (platform-dependent)
void UserInterface::clearScreen() {
    system("cls");
}

// Wait for a keypress
void UserInterface::waitForKeypress() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}