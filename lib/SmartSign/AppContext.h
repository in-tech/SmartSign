#pragma once
#include "Version.h"
#include "SettingsManager.h"
#include "NetworkManager.h"
#include "AzureClient.h"
#include "PowerManager.h"
#include "InputManager.h"
#include "Display.h"
#include "Navigator.h"
#include "Log.h"

// permission bits
#define PERM_AUTHORIZATION    1
#define PERM_WRITE_SETTINGS   2
#define PERM_ADMINISTRATION   (PERM_AUTHORIZATION | PERM_WRITE_SETTINGS)

class AppContext
{
public:
    AppContext();
    ~AppContext();

    uint8_t GetPermissions(const String& cardId);
    bool HasPermissions(const uint8_t permissionBits);

    GlobalSettings& Settings();
    NetworkManager& GetNetworkMgr();
    AzureClient& GetAzureClient();
    PowerManager& GetPowerMgr();
    InputManager& GetInputMgr();
    Display& GetDisplay();
    Navigator& GetNavigator();

    void Init();
    void Update();
    void DeInit();

private:
    SettingsManager _settingsMgr;
    NetworkManager _networkMgr;
    AzureClient _azureClient;
    PowerManager _powerMgr;
    InputManager _inputMgr;
    Display _display;
    Navigator _navigator;
};