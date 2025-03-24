// Global Time Zone App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <limits>
#include "TimeZone.h"
#include "DateTime.h"
#include "TimeZoneManager.h"
#include "UserInterface.h"

int main() {
    // Create the time zone manager
    TimeZoneManager tzManager;

    // Create the user interface
    UserInterface ui(tzManager);

    int choice;
    bool flag = true;

    while (flag) {
        ui.displayMainMenu();

        std::cout << "Enter your choice: ";
        std::cin >> choice;

        // Clear input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        ui.clearScreen();

        switch (choice) {
        case 1:
            ui.viewCurrentTime();
            break;
        case 2:
            ui.convertTime();
            break;
        case 3:
            ui.manageFavorites();
            break;
        case 4:
            tzManager.toggleDST();
            std::cout << "Daylight Savings Time adjustments turned "
                << (tzManager.isDSTEnabled() ? "ON" : "OFF") << std::endl;
            ui.waitForKeypress();
            break;
        case 5:
            ui.showHelp();
            break;
        case 6:
            flag = false;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            ui.waitForKeypress();
        }
    }

    std::cout << "Thank you for using the Global Time Zone Application!" << std::endl;
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
