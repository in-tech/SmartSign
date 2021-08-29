#pragma once
#include "Version.h"
#include "GlobalSettings.h"
#include "INetworkManager.h"
#include "IAzureClient.h"
#include "IPowerManager.h"
#include "IInputManager.h"
#include "IDisplay.h"
#include "INavigator.h"

// permission bits
#define PERM_AUTHORIZATION    1
#define PERM_WRITE_SETTINGS   2
#define PERM_ADMINISTRATION   (PERM_AUTHORIZATION | PERM_WRITE_SETTINGS)

class IAppContext
{
public:
    virtual ~IAppContext() {};

    virtual uint8_t GetPermissions(const String& cardId) = 0;
    virtual bool HasPermissions(const uint8_t permissionBits) = 0;

    virtual GlobalSettings& Settings() = 0;
    virtual INetworkManager& GetNetworkMgr() = 0;
    virtual IAzureClient& GetAzureClient() = 0;
    virtual IPowerManager& GetPowerMgr() = 0;
    virtual IInputManager& GetInputMgr() = 0;
    virtual IDisplay& GetDisplay() = 0;
    virtual INavigator& GetNavigator() = 0;

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void DeInit() = 0;
};