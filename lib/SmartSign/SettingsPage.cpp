#include "SettingsPage.h"
#include "AppContext.h"
#include "CryptoUtils.h"

SettingsPage::SettingsPage(AppContext& ctx) :
    NavPage(ctx)
{
    const String ssid = String("SmartSign-") + CryptoUtils::RandomString("ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890", 4);
    const String key = CryptoUtils::RandomString("abcdefghijklmnopqrstuvwxyz01234567890", 14);
    _ctx.GetNetworkMgr().StartConfigurator(ssid, key),
    _ctx.GetDisplay().ShowSettingsScreen(ssid, key, "", "Back");
}

SettingsPage::~SettingsPage()
{    
}

void SettingsPage::Update(const InputState& inputState)
{
    _ctx.GetPowerMgr().SetStatusLED((millis() / 500) % 2 == 0);
    
    if (!_ctx.HasPermissions(PERM_WRITE_SETTINGS) || inputState.PressedButtonB || !_ctx.GetNetworkMgr().ConfiguratorIsRunning())
    {
        _ctx.GetNetworkMgr().StopConfigurator();
        _ctx.GetPowerMgr().SetStatusLED(false);
        GoTo(NAV_PAGE_ADMIN_MENU);
    }
}