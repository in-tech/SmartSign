#pragma once
#include <vector>
#include "Epd.h"
#include <epdpaint.h>
#include "IDisplay.h"

class IAppContext;

class Display : public IDisplay
{
public:
    Display(IAppContext& ctx);
    virtual ~Display() override;

    void Init();
    void DeInit();

    virtual void ShowScheduleScreen(const time_t localNow, const std::vector<ScheduleItem>& items) override;
    virtual void ShowAdminMenuScreen(const String& btnInfoA, const String& btnInfoB) override;
    virtual void ShowSettingsScreen(const String& wifiSsid, const String& wifiKey, const String& btnInfoA, const String& btnInfoB) override;
    virtual void ShowAuthorizationScreen(const String& message, const String& uri, const String& code, const String& btnInfoA, const String& btnInfoB) override;
    virtual void ShowUnknownCardScreen(const String& header, const String& code, const String& btnInfoA, const String& btnInfoB) override;
    virtual void ShowInfoScreen(const String& info, const String& btnInfoA, const String& btnInfoB) override;
    virtual void ShowErrorScreen(const String& error, const String& btnInfoA, const String& btnInfoB) override;
    virtual void ShowFontTestScreen() override;
    virtual bool IsBusy() override;

private:
    void RenderFirmwareVersion();
    void RenderQRCode(const String& message, const int x, const int y, const int size, const int version = 3);
    void RenderCwaEventQRCode(const ScheduleItem& item);
    void RenderButtonInfos(const String& btnInfoA, const String& btnInfoB, const int color);
    bool PrepareHash(const String& content);
    void Present(const bool fastMode);

private:
    IAppContext& _ctx;   
    std::vector<uint8_t> _frameBuffer;
    Paint _paint;
    Epd _epd;
    bool _active;
};