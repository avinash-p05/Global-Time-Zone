// main.cpp
// Entry point for the Time Zone Application
// Visual Studio C++14 compatible version

#include "TimeZoneApp.h"
#include <iostream>
#include <string>
#include <direct.h>
#include <sys/stat.h>

//The path.c_str() converts the std::string to a C-style string (const char*),
// which is the format _mkdir expects.

// Check if a directory exists
bool directoryExists(const std::string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}
struct stat info;

//Declares a variable info of type struct stat.This structure is used to store information about the file or directory.
//stat(path.c_str(), &info)
//
//Calls the stat function, which retrieves information about the file or directory specified by path.
//path.c_str() converts the std::string to a C - style string(const char*), which is the format stat expects.
//The stat function fills the info structure with information about the file or directory.
//== 0
//
//Checks if the stat function call was successful.stat returns 0 if it successfully 
// retrieves the information, and a non - zero value if it fails(e.g., if the file or directory does not exist).
//(info.st_mode & S_IFDIR)

/*Checks if the st_mode field of the info structure indicates 
that the path is a directory. S_IFDIR is a macro that represents 
the directory bit in the st_mode field*/

// Create directory
bool createDirectory(const std::string& path) {
    return _mkdir(path.c_str()) == 0;
}

int main() {
    std::cout << "Global Time Zone Application v2.0" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Using TimeZoneDB API for time zone data" << std::endl;
    std::cout << std::endl;

    try {
        // Replace with your actual API key from TimeZoneDB
        const std::string API_KEY = "SRYQU1B6TP23";

        // Create data directory if it doesn't exist
        if (!directoryExists("data")) {
            createDirectory("data");
        }

        // Create the application with config file in the data directory
        TimeZoneApp app(API_KEY, "data/user_config.json");

        // Initialize the application
        if (!app.initialize()) {
            std::cerr << "Failed to initialize the application. Exiting." << std::endl;
            return 1;
        }

        // Run the main application loop
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        system("pause"); // Keep console window open
        return 1;
    }
    catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        system("pause"); // Keep console window open
        return 1;
    }

    return 0;
}