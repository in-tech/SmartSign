#pragma once
#include "SchedulePage.h"

class IDisplay
{
public:
    virtual ~IDisplay() {};

    virtual void ShowScheduleScreen(const time_t localNow, const std::vector<ScheduleItem>& items) = 0;
    virtual void ShowAdminMenuScreen(const String& btnInfoA, const String& btnInfoB) = 0;
    virtual void ShowSettingsScreen(const String& wifiSsid, const String& wifiKey, const String& btnInfoA, const String& btnInfoB) = 0;
    virtual void ShowAuthorizationScreen(const String& message, const String& uri, const String& code, const String& btnInfoA, const String& btnInfoB) = 0;
    virtual void ShowUnknownCardScreen(const String& header, const String& code, const String& btnInfoA, const String& btnInfoB) = 0;
    virtual void ShowInfoScreen(const String& info, const String& btnInfoA, const String& btnInfoB) = 0;
    virtual void ShowErrorScreen(const String& error, const String& btnInfoA, const String& btnInfoB) = 0;
    virtual void ShowFontTestScreen() = 0;
    virtual bool IsBusy() = 0;
};