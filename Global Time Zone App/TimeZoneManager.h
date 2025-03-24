#ifndef TIMEZONEMANAGER_H
#define TIMEZONEMANAGER_H

#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include "TimeZone.h"
#include "DateTime.h"



class TimeZoneManager {
private:
    std::vector<TimeZone> timeZones;
    std::vector<std::string> favoriteTimeZones;
    bool dstEnabled;

public:
    TimeZoneManager() : dstEnabled(true) {
        initializeTimeZones();
    }

    // Initialize predefined time zones
    void initializeTimeZones();

    // Get all time zones
    const std::vector<TimeZone>& getAllTimeZones() const;

    // Get favorite time zones
    const std::vector<std::string>& getFavoriteTimeZones() const;

    // Add a time zone to favorites
    bool addFavorite(const std::string& zoneName);

    // Remove a time zone from favorites
    bool removeFavorite(int index);

    // Toggle DST enabled setting
    void toggleDST();

    // Get DST enabled status
    bool isDSTEnabled() const;

    // Find a time zone by name
    TimeZone* findTimeZone(const std::string& name);

    // Check if DST is active for a given date in a time zone
    bool isDSTActive(const TimeZone& tz, int month, int day) const;

    // Get current time in the specified time zone
    DateTime getCurrentTime(const std::string& timeZoneName);

    // Convert time from one zone to another
    DateTime convertTimeZone(const DateTime& sourceTime, const std::string& targetZone);
};

#endif // TIMEZONEMANAGER_H