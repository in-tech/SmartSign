#pragma once
#include "IAppContext.h"
#include "SettingsManager.h"
#include "NetworkManager.h"
#include "AzureClient.h"
#include "PowerManager.h"
#include "InputManager.h"
#include "Display.h"
#include "Navigator.h"
#include "Log.h"

class AppContext : public IAppContext
{
public:
    AppContext();
    virtual ~AppContext() override;

    virtual uint8_t GetPermissions(const String& cardId) override;
    virtual bool HasPermissions(const uint8_t permissionBits) override;

    virtual GlobalSettings& Settings() override;
    virtual INetworkManager& GetNetworkMgr() override;
    virtual IAzureClient& GetAzureClient() override;
    virtual IPowerManager& GetPowerMgr() override;
    virtual IInputManager& GetInputMgr() override;
    virtual IDisplay& GetDisplay() override;
    virtual INavigator& GetNavigator() override;

    virtual void Init() override;
    virtual void Update() override;
    virtual void DeInit() override;

private:
    SettingsManager _settingsMgr;
    NetworkManager _networkMgr;
    AzureClient _azureClient;
    PowerManager _powerMgr;
    InputManager _inputMgr;
    Display _display;
    Navigator _navigator;
};