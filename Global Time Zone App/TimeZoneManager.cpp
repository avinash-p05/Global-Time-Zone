#include "TimeZoneManager.h"


// Initialize predefined time zones
void TimeZoneManager::initializeTimeZones() {
    // Format: name, region, UTC offset in minutes, has DST, DST offset, DST start, DST end
    timeZones.push_back({ "UTC", "Worldwide", 0, false, 0, "", "" });
    timeZones.push_back({ "EST", "Eastern America", -300, true, 60, "3,10", "11,3" });
    timeZones.push_back({ "IST", "India", 330, false, 0, "", "" });
    timeZones.push_back({ "JST", "Japan", 540, false, 0, "", "" });
    timeZones.push_back({ "AEST", "Eastern Australia", 600, true, 60, "10,1", "4,1" });

    // Add a few common cities mapped to time zones
    timeZones.push_back({ "New York", "USA", -300, true, 60, "3,10", "11,3" });
    timeZones.push_back({ "Chicago", "USA", -360, true, 60, "3,10", "11,3" });
    timeZones.push_back({ "Mumbai", "India", 330, false, 0, "", "" });
    timeZones.push_back({ "Tokyo", "Japan", 540, false, 0, "", "" });
    timeZones.push_back({ "Sydney", "Australia", 600, true, 60, "10,1", "4,1" });
}

// Get all time zones
const std::vector<TimeZone>& TimeZoneManager::getAllTimeZones() const {
    return timeZones;
}

// Get favorite time zones
const std::vector<std::string>& TimeZoneManager::getFavoriteTimeZones() const {
    return favoriteTimeZones;
}

// Add a time zone to favorites
bool TimeZoneManager::addFavorite(const std::string& zoneName) {
    if (findTimeZone(zoneName)) {
        // Check if already in favorites
        if (std::find(favoriteTimeZones.begin(), favoriteTimeZones.end(), zoneName)
            == favoriteTimeZones.end()) {
            favoriteTimeZones.push_back(zoneName);
            return true;
        }
    }
    return false;
}

// Remove a time zone from favorites
bool TimeZoneManager::removeFavorite(int index) {
    if (index >= 0 && index < static_cast<int>(favoriteTimeZones.size())) {
        favoriteTimeZones.erase(favoriteTimeZones.begin() + index);
        return true;
    }
    return false;
}

// Toggle DST enabled setting
void TimeZoneManager::toggleDST() {
    dstEnabled = !dstEnabled;
}

// Get DST enabled status
bool TimeZoneManager::isDSTEnabled() const {
    return dstEnabled;
}

// Find a time zone by name
TimeZone* TimeZoneManager::findTimeZone(const std::string& name) {
    for (auto& tz : timeZones) {
        if (tz.name == name) {
            return &tz;
        }
    }

    // Try case-insensitive search
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (auto& tz : timeZones) {
        std::string lowerTzName = tz.name;
        std::transform(lowerTzName.begin(), lowerTzName.end(), lowerTzName.begin(), ::tolower);

        if (lowerTzName == lowerName) {
            return &tz;
        }
    }

    return nullptr;
}

// Check if DST is active for a given date in a time zone
bool TimeZoneManager::isDSTActive(const TimeZone& tz, int month, int day) const {
    if (!tz.hasDST || tz.startDST.empty() || tz.endDST.empty()) {
        return false;
    }

    // Parse DST start/end dates
    int startMonth = std::stoi(tz.startDST.substr(0, tz.startDST.find(',')));
    int startDay = std::stoi(tz.startDST.substr(tz.startDST.find(',') + 1));
    int endMonth = std::stoi(tz.endDST.substr(0, tz.endDST.find(',')));
    int endDay = std::stoi(tz.endDST.substr(tz.endDST.find(',') + 1));

    // Northern hemisphere (DST typically Mar-Nov)
    if (startMonth < endMonth) {
        if (month > startMonth && month < endMonth) {
            return true;
        }
        if (month == startMonth && day >= startDay) {
            return true;
        }
        if (month == endMonth && day < endDay) {
            return true;
        }
    }
    // Southern hemisphere (DST typically Oct-Apr)
    else {
        if (month > startMonth || month < endMonth) {
            return true;
        }
        if (month == startMonth && day >= startDay) {
            return true;
        }
        if (month == endMonth && day < endDay) {
            return true;
        }
    }

    return false;
}

// Get current time in the specified time zone
DateTime TimeZoneManager::getCurrentTime(const std::string& timeZoneName) {
    TimeZone* tz = findTimeZone(timeZoneName);
    if (!tz) {
        // Default to UTC if time zone not found
        tz = findTimeZone("UTC");
    }

    // Get current time in UTC
    time_t now = time(nullptr);
    tm utcTime;
    gmtime_s(&utcTime, &now);

    // Apply time zone offset
    int totalMinutes = utcTime.tm_hour * 60 + utcTime.tm_min + tz->utcOffset;

    // Check if DST is active and apply if enabled
    bool isDST = false;
    if (dstEnabled && tz->hasDST) {
        isDST = isDSTActive(*tz, utcTime.tm_mon + 1, utcTime.tm_mday);
        if (isDST) {
            totalMinutes += tz->dstOffset;
        }
    }

    // Normalize minutes to hours and days
    int days = 0;
    while (totalMinutes < 0) {
        totalMinutes += 1440; // 24 hours in minutes
        days--;
    }
    while (totalMinutes >= 1440) {
        totalMinutes -= 1440;
        days++;
    }

    int hour = totalMinutes / 60;
    int minute = totalMinutes % 60;

    // Adjust date if needed
    tm adjustedDate = utcTime;
    adjustedDate.tm_mday += days;
    mktime(&adjustedDate); // Normalize the date

    DateTime result = {
        adjustedDate.tm_year + 1900,
        adjustedDate.tm_mon + 1,
        adjustedDate.tm_mday,
        hour,
        minute,
        timeZoneName,
        isDST
    };

    return result;

}

// Convert time from one zone to another
DateTime TimeZoneManager::convertTimeZone(const DateTime& sourceTime, const std::string& targetZone) {
    TimeZone* sourceTZ = findTimeZone(sourceTime.timeZoneName);
    TimeZone* targetTZ = findTimeZone(targetZone);

    if (!sourceTZ || !targetTZ) {
        // Error case, return source time unchanged
        return sourceTime;
    }

    // Convert to UTC time in minutes
    int sourceOffset = sourceTZ->utcOffset;
    if (dstEnabled && sourceTime.isDST) {
        sourceOffset += sourceTZ->dstOffset;
    }

    // Calculate total minutes in UTC
    tm sourceDate = { 0 };
    sourceDate.tm_year = sourceTime.year - 1900;
    sourceDate.tm_mon = sourceTime.month - 1;
    sourceDate.tm_mday = sourceTime.day;
    sourceDate.tm_hour = sourceTime.hour;
    sourceDate.tm_min = sourceTime.minute;

    // Convert date to time_t
    time_t sourceTimeT = mktime(&sourceDate);

    // Adjust for source time zone to get UTC
    sourceTimeT -= sourceOffset * 60;

    // Convert target offset
    int targetOffset = targetTZ->utcOffset;
    bool targetIsDST = false;

    // Get target date components for DST check
    tm targetDate;
    gmtime_s(&targetDate, &sourceTimeT);

    if (dstEnabled && targetTZ->hasDST) {
        targetIsDST = isDSTActive(*targetTZ, targetDate.tm_mon + 1, targetDate.tm_mday);
        if (targetIsDST) {
            targetOffset += targetTZ->dstOffset;
        }
    }

    // Apply target offset
    sourceTimeT += targetOffset * 60;

    // Convert back to tm struct
    tm resultTime;
    gmtime_s(&resultTime, &sourceTimeT);

    DateTime result = {
        resultTime.tm_year + 1900,
        resultTime.tm_mon + 1,
        resultTime.tm_mday,
        resultTime.tm_hour,
        resultTime.tm_min,
        targetZone,
        targetIsDST
    };

    return result;
}
