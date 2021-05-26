#pragma once
#include <vector>
#include "Epd.h"
#include <epdpaint.h>
#include "AzureClient.h"
#include "SchedulePage.h"

class AppContext;

class Display
{
public:
    Display(AppContext& ctx);
    ~Display();

    void Init();
    void ShowScheduleScreen(const time_t localNow, const std::vector<ScheduleItem>& items);
    void ShowAdminMenuScreen(const String& btnInfoA, const String& btnInfoB);
    void ShowSettingsScreen(const String& wifiSsid, const String& wifiKey, const String& btnInfoA, const String& btnInfoB);
    void ShowAuthorizationScreen(const String& message, const String& uri, const String& code, const String& btnInfoA, const String& btnInfoB);
    void ShowUnknownCardScreen(const String& header, const String& code, const String& btnInfoA, const String& btnInfoB);
    void ShowInfoScreen(const String& info, const String& btnInfoA, const String& btnInfoB);
    void ShowErrorScreen(const String& error, const String& btnInfoA, const String& btnInfoB);
    void ShowFontTestScreen();
    bool IsBusy();
    void DeInit();

private:
    void RenderQRCode(const String& message, const int x, const int y, const int size, const int version = 3);
    void RenderCwaEventQRCode(const ScheduleItem& item);
    void RenderButtonInfos(const String& btnInfoA, const String& btnInfoB, const int color);
    bool PrepareHash(const String& content);
    void Present(const bool fastMode);

private:
    AppContext& _ctx;   
    std::vector<uint8_t> _frameBuffer;
    Paint _paint;
    Epd _epd;
    bool _active;
};