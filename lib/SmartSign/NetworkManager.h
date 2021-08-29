#pragma once
#include <Esp32Foundation.h>
#include <NTPClient.h>
#include "INetworkManager.h"

class IAppContext;

class NetworkManager : public INetworkManager
{
public:
    NetworkManager(IAppContext& ctx);
    ~NetworkManager();

    void Init();
    void DeInit();
    void Update();

    virtual bool IsPending() override;

    virtual bool WifiIsConfigured() override;
    virtual bool WifiIsConnected() override;

    virtual bool TimeIsSynchronized() override;
    virtual bool TimeIsValid() override;
    virtual bool TryGetUtcTime(time_t& time) override;

    virtual void StartConfigurator(const String& ssid, const String& key) override;
    virtual void StopConfigurator() override;
    virtual bool ConfiguratorIsRunning() override;

private:
    void SyncTime();

private:
    IAppContext& _ctx;
    String _lastWifiSsid;
    esp32::foundation::WiFiSmartClient _wifiClient;
    esp32::foundation::HtmlConfigurator _configurator;
    NTPClient _ntpClient;
    bool _timeSynchronized;
};