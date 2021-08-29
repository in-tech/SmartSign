#pragma once
#include <Arduino.h>
#include <TimeLib.h>

class INetworkManager
{
public:
    virtual ~INetworkManager() {};

    virtual bool IsPending() = 0;

    virtual bool WifiIsConfigured() = 0;
    virtual bool WifiIsConnected() = 0;

    virtual bool TimeIsSynchronized() = 0;
    virtual bool TimeIsValid() = 0;
    virtual bool TryGetUtcTime(time_t& time) = 0;

    virtual void StartConfigurator(const String& ssid, const String& key) = 0;
    virtual void StopConfigurator() = 0;
    virtual bool ConfiguratorIsRunning() = 0;
};