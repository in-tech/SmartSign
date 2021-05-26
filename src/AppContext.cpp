#include "AppContext.h"

AppContext::AppContext() :
    _settingsMgr(*this),
    _networkMgr(*this),
    _azureClient(*this),
    _powerMgr(*this),    
    _inputMgr(*this),
    _display(*this),
    _navigator(*this)
{
}

AppContext::~AppContext()
{
}

uint8_t AppContext::GetPermissions(const String& cardId)
{
    if (!cardId.isEmpty())
    {
        if (cardId.equalsIgnoreCase(ADMIN_CARD_ID))
        {
            return PERM_ADMINISTRATION;
        }

        if (cardId.equalsIgnoreCase(Settings().UserCardId))
        {
            return PERM_AUTHORIZATION;
        }
    }

    return 0;
}

bool AppContext::HasPermissions(const uint8_t permissionBits)
{
    return (GetPermissions(_inputMgr.State().SmartCardId) & permissionBits) == permissionBits; 
}

GlobalSettings& AppContext::Settings()
{
    return _settingsMgr.Settings();
}

NetworkManager& AppContext::GetNetworkMgr()
{
    return _networkMgr;
}

AzureClient& AppContext::GetAzureClient()
{
    return _azureClient;
}

PowerManager& AppContext::GetPowerMgr()
{
    return _powerMgr;
}

InputManager& AppContext::GetInputMgr()
{
    return _inputMgr;
}

Display& AppContext::GetDisplay()
{
    return _display;
}

Navigator& AppContext::GetNavigator()
{
    return _navigator;
}

void AppContext::Init()
{
    _settingsMgr.Init();
    _networkMgr.Init();
    _azureClient.Init();
    _powerMgr.Init();
    _inputMgr.Init();
    _display.Init();    
    _navigator.Init();
}

void AppContext::Update()
{
    _settingsMgr.Update();
    _networkMgr.Update();
    _azureClient.Update();
    _powerMgr.Update();
    _inputMgr.Update();
    _navigator.Update();
}

void AppContext::DeInit()
{
    _networkMgr.DeInit();
    _display.DeInit();
}