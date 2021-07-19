#pragma once
#include <Arduino.h>
#include <TimeLib.h>

class TimeUtils
{
public:
    static time_t ToLocalTime(const time_t utcTime);
    static time_t ToUtcTime(const time_t localTime);
    static String ToIsoString(const time_t utcTime);
    static String ToDateString(const time_t time);
    static String ToShortTimeString(const time_t time);
    static time_t UtcIsoStringToTime(const String& utcTimeStr);
};