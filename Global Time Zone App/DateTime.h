#ifndef DATETIME_H
#define DATETIME_H

#include <string>

// Structure for my time display
struct DateTime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    std::string timeZoneName;
    bool isDST;
};

#endif // DATETIME_H