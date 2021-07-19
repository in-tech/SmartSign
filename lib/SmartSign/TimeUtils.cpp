#include "TimeUtils.h"
#include "Config.h"
#include <Timezone.h>

// Australia Eastern Time Zone (Sydney, Melbourne)
TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660}; // UTC + 11 hours
TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600}; // UTC + 10 hours
Timezone ausET(aEDT, aEST);

// Moscow Standard Time (MSK, does not observe DST)
TimeChangeRule msk = {"MSK", Last, Sun, Mar, 1, 180};
Timezone tzMSK(msk);

// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};   // Central European Standard Time
Timezone CE(CEST, CET);

// United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60}; // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};  // Standard Time
Timezone UK(BST, GMT);

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240}; // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};  // Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);

// US Central Time Zone (Chicago, Houston)
TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, Sun, Nov, 2, -360};
Timezone usCT(usCDT, usCST);

// US Mountain Time Zone (Denver, Salt Lake City)
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};
Timezone usMT(usMDT, usMST);

// Arizona is US Mountain Time Zone but does not use DST
Timezone usAZ(usMST);

// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);

time_t TimeUtils::ToLocalTime(const time_t utcTime)
{
    return TIMEZONE.toLocal(utcTime);
}

time_t TimeUtils::ToUtcTime(const time_t localTime)
{
    return TIMEZONE.toUTC(localTime);
}

String TimeUtils::ToIsoString(const time_t utcTime)
{
    char str[32];
    snprintf(str, 32, "%04d-%02d-%02dT%02d:%02d:%02d",
             year(utcTime), month(utcTime), day(utcTime),
             hour(utcTime), minute(utcTime), second(utcTime));
    return str;
}

String TimeUtils::ToDateString(const time_t time)
{
    char str[32];
    snprintf(str, 32, "%02d.%02d.%04d", day(time), month(time), year(time));
    return str;
}

String TimeUtils::ToShortTimeString(const time_t time)
{
    char str[32];
    snprintf(str, 32, "%02d:%02d", hour(time), minute(time));
    return str;
}

time_t TimeUtils::UtcIsoStringToTime(const String& utcTimeStr)
{
    if (utcTimeStr.length() >= 19)
    {
        tmElements_t t = {};
        t.Year = CalendarYrToTm(utcTimeStr.substring(0, 4).toInt());
        t.Month = utcTimeStr.substring(5, 5 + 2).toInt();
        t.Day = utcTimeStr.substring(8, 8 + 2).toInt();
        t.Hour = utcTimeStr.substring(11, 11 + 2).toInt();
        t.Minute = utcTimeStr.substring(14, 14 + 2).toInt();
        t.Second = utcTimeStr.substring(17, 17 + 2).toInt();
        return makeTime(t);
    }    
    return 0;
}