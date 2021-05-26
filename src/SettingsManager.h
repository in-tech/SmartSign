#pragma once
#include "GlobalSettings.h"

class AppContext;

class SettingsManager
{
public:
    SettingsManager(AppContext& ctx);
    ~SettingsManager();

    void Init();
    void Update();

    GlobalSettings& Settings();

private:
    AppContext& _ctx;
    SerialCLI _serialCli;
    GlobalSettings _settings;
};