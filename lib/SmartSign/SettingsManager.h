#pragma once
#include "GlobalSettings.h"

class IAppContext;

class SettingsManager
{
public:
    SettingsManager(IAppContext& ctx);
    ~SettingsManager();

    void Init();
    void Update();

    GlobalSettings& Settings();

private:
    IAppContext& _ctx;
    SerialCLI _serialCli;
    GlobalSettings _settings;
};