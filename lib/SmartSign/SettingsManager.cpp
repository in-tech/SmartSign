#include "SettingsManager.h"
#include "AppContext.h"

SettingsManager::SettingsManager(AppContext& ctx) :
    _ctx(ctx)
{
}

SettingsManager::~SettingsManager()
{
}

void SettingsManager::Init()
{
    _serialCli.On("help", [&]() -> bool
    {
        _serialCli.PrintCommands();
        return true;
    }, "Prints this help.");

    _serialCli.On("restart", [&]() -> bool
    {
        ESP.restart();
        return true;
    }, "Restart the device.");

    _serialCli.On("params", [&]() -> bool
    {
        DefaultParameterSet.PrintParameters(Serial, true);
        return true;
    }, "List parameters.");

    _serialCli.On("set", [&](const String& arg0, const String& arg1) -> bool
    {
        auto parameter = DefaultParameterSet.GetParameter(arg0);
        if (parameter != nullptr)
        {
            parameter->SetFromString(arg1);
            DefaultParameterSet.Save();
            return true;
        }
        return false;
    }, "Set parameter. (e.g. set hostname SmartSign)");

    _serialCli.On("delete", [&](const String& arg) -> bool
    {
        DefaultParameterSet.Unset(arg);
        DefaultParameterSet.Save();
        return true;
    }, "Delete parameter. (e.g. delete hostname)");
}

void SettingsManager::Update()
{
    if (_ctx.HasPermissions(PERM_ADMINISTRATION))
    {
        _serialCli.Update();
    }
}

GlobalSettings& SettingsManager::Settings()
{
    return _settings;
}