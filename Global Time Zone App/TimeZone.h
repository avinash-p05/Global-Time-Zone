#ifndef TIMEZONE_H
#define TIMEZONE_H

#include <string>

// Structure to represent a time zone
struct TimeZone {
    std::string name;
    std::string region;
    int utcOffset; // in minutes
    bool hasDST;
    int dstOffset; // additional minutes during DST
    std::string startDST; // simplified as month,day (e.g., "3,10" for March 10)
    std::string endDST;   // simplified as month,day (e.g., "11,3" for November 3)
};

#endif // TIMEZONE_H