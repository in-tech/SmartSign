#pragma once
#include <Arduino.h>
#include "IAppContext.h"
#include "PowerManager.h"
#include "InputManager.h"

class NetworkManagerMock1 : public INetworkManager
{
    bool IsPending() { return false; };
    bool WifiIsConfigured() { return true; };
    bool WifiIsConnected() { return true; };
    bool TimeIsSynchronized() { return true; };
    bool TimeIsValid() { return true; };
    bool TryGetUtcTime(time_t &time)
    {
        time = millis() / 1000;
        return true;
    }
    void StartConfigurator(const String &ssid, const String &key){};
    void StopConfigurator(){};
    bool ConfiguratorIsRunning() { return false; };
};

class AzureClientMock1 : public IAzureClient
{
    AuthorizationProgress GetAuthorizationProgress() { return AUTH_PROG_UNKNOWN; };
    String GetAuthorizationError() { return "not implemented"; };
    AuthorizationRequestInfo BeginAuthorization() { return AuthorizationRequestInfo{false, "not implemented", "", "", "", ""}; }
    void CancelAuthorization(const String &reason){};
    GetScheduleResponse GetSchedule(DynamicJsonDocument &json) { return GetScheduleResponse{false, "not implemented"}; };
};

class DisplayMock1 : public IDisplay
{
    void ShowScheduleScreen(const time_t localNow, const std::vector<ScheduleItem> &items){};
    void ShowAdminMenuScreen(const String &btnInfoA, const String &btnInfoB){};
    void ShowSettingsScreen(const String &wifiSsid, const String &wifiKey, const String &btnInfoA, const String &btnInfoB){};
    void ShowAuthorizationScreen(const String &message, const String &uri, const String &code, const String &btnInfoA, const String &btnInfoB){};
    void ShowUnknownCardScreen(const String &header, const String &code, const String &btnInfoA, const String &btnInfoB){};
    void ShowInfoScreen(const String &info, const String &btnInfoA, const String &btnInfoB){};
    void ShowErrorScreen(const String &error, const String &btnInfoA, const String &btnInfoB){};
    void ShowFontTestScreen(){};
    bool IsBusy() { return false; };
};

class NavigatorMock1 : public INavigator
{
    void GoTo(const NavPages page) {}
};

class AppContextMock1 : public IAppContext
{
public:
    AppContextMock1()
        : _powerMgr(*this), _inputMgr(*this)
    {
    }

    ~AppContextMock1()
    {
        DeInit();
    }

    uint8_t GetPermissions(const String &cardId) { return cardId.isEmpty() ? 0 : PERM_ADMINISTRATION; };
    bool HasPermissions(const uint8_t permissionBits) { return false; };

    GlobalSettings &Settings() { return _settings; };
    INetworkManager &GetNetworkMgr() { return _networkMgrMock; };
    IAzureClient &GetAzureClient() { return _azureClientMock; };
    IPowerManager &GetPowerMgr() { return _powerMgr; };
    IInputManager &GetInputMgr() { return _inputMgr; };
    IDisplay &GetDisplay() { return _displayMock; };
    INavigator &GetNavigator() { return _navigatorMock; };

    void Init()
    {
        _powerMgr.Init();
        _inputMgr.Init();
    };

    void Update()
    {
        _powerMgr.Update();
        _inputMgr.Update();
    };

    void DeInit(){};

private:
    GlobalSettings _settings;
    NetworkManagerMock1 _networkMgrMock;
    AzureClientMock1 _azureClientMock;
    DisplayMock1 _displayMock;
    NavigatorMock1 _navigatorMock;

    PowerManager _powerMgr;
    InputManager _inputMgr;
};