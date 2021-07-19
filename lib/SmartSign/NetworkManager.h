#pragma once
#include <Esp32Foundation.h>
#include <NTPClient.h>
#include <TimeLib.h>

class AppContext;

class NetworkManager
{
public:
    NetworkManager(AppContext& ctx);
    ~NetworkManager();

    void Init();
    void DeInit();
    void Update();

    bool IsPending();

    bool WifiIsConfigured();
    bool WifiIsConnected();

    bool TimeIsSynchronized();
    bool TimeIsValid();
    bool TryGetUtcTime(time_t& time);

    void StartConfigurator(const String& ssid, const String& key);
    void StopConfigurator();
    bool ConfiguratorIsRunning();

private:
    void SyncTime();

private:
    AppContext& _ctx;
    String _lastWifiSsid;
    esp32::foundation::WiFiSmartClient _wifiClient;
    esp32::foundation::HtmlConfigurator _configurator;
    NTPClient _ntpClient;
    bool _timeSynchronized;
};