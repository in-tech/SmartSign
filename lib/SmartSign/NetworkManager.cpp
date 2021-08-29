#include "NetworkManager.h"
#include "IAppContext.h"
#include "Log.h"

WiFiUDP ntpUDP;

NetworkManager::NetworkManager(IAppContext &ctx) : _ctx(ctx),
                                                  _ntpClient(ntpUDP),
                                                  _timeSynchronized(false)
{
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::Init()
{
    Update();
}

void NetworkManager::DeInit()
{
    if (ConfiguratorIsRunning())
    {
        StopConfigurator();
    }
    else
    {
        _wifiClient.Disconnect();
    }
}

void NetworkManager::Update()
{
    if (ConfiguratorIsRunning())
    {
        _configurator.Update();
    }
    else
    {
        const String wifiSsid = _ctx.Settings().WifiSSID;
        if (!wifiSsid.isEmpty() && wifiSsid != _lastWifiSsid)
        {
            Log::Info(String("Connecting to: ") + wifiSsid);
        }
        _lastWifiSsid = wifiSsid;

        if (_wifiClient.Connect(
                wifiSsid,
                _ctx.Settings().WifiKey,
                _ctx.Settings().HostName, 1))
        {
            SyncTime();
        }
    }
}

bool NetworkManager::IsPending()
{
    if (WifiIsConnected() && TimeIsSynchronized())
    {
        return false;
    }

    if (_ctx.Settings().WifiSSID.ToString().isEmpty())
    {
        return false;
    }

    return millis() < WIFI_TIMEOUT;
}

bool NetworkManager::WifiIsConfigured()
{
    return !_ctx.Settings().WifiSSID.ToString().isEmpty();
}

bool NetworkManager::WifiIsConnected()
{
    return _wifiClient.Connected();
}

bool NetworkManager::TimeIsSynchronized()
{
    return _timeSynchronized;
}

bool NetworkManager::TimeIsValid()
{
    time_t t;
    return TryGetUtcTime(t);
}

bool NetworkManager::TryGetUtcTime(time_t &time)
{
    if (!_timeSynchronized)
    {
        SyncTime();
    }

    if (_timeSynchronized)
    {
        time = _ntpClient.getEpochTime();
        return true;
    }

    time_t wakupTime = 0;
    if (_ctx.GetPowerMgr().TryGetUtcWeakupTime(wakupTime))
    {
        time = wakupTime + (millis() + 500) / 1000;
        Log::Warning(String("using extrapolated utc-time: ") + TimeUtils::ToIsoString(time));
        return true;
    }

    time = 0;
    return false;
}

void NetworkManager::SyncTime()
{
    if (WifiIsConnected())
    {
        _timeSynchronized = _ntpClient.update();
        if (_timeSynchronized)
        {
            setTime(_ntpClient.getEpochTime());
        }
    }
}

void NetworkManager::StartConfigurator(const String &ssid, const String &key)
{
    _wifiClient.Disconnect();
    if (!_configurator.Start(ssid, key, "SmartSign"))
    {
        Log::Error("failed to start captive-portal");
    }
}

void NetworkManager::StopConfigurator()
{
    _configurator.Stop();
}

bool NetworkManager::ConfiguratorIsRunning()
{
    return !_configurator.IsStopped();
}