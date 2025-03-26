#pragma once
// TimeZone.h
// Definition of the TimeZone structure
#include <string>

struct TimeZone {
    std::string code;         // time zone code (e.g., "Asia/Kolkata")
    std::string name;         // display name (e.g., "Kolkata")
    std::string region;       // region information (e.g., "India")
    double offset;            // UTC offset in hours (e.g., +5.5)
    bool hasDST;              // is DST is used in this zone
    bool dstActive;           // is DST is currently active
    std::string formattedTime; // current time as formatted string from API
    time_t timestamp;         // current timestamp from API

    // Default constructor
    TimeZone() : offset(0.0), hasDST(false), dstActive(false), timestamp(0) {}
};
