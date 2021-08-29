#pragma once
#include <IPowerManager.h>
#include "Config.h"

class IAppContext;

class PowerManager : public IPowerManager
{
public:
    PowerManager(IAppContext& ctx);
    virtual ~PowerManager() override;

    void Init();
    void Update();

    virtual void EnterDeepSleep(const time_t maxUtcWakeTime = 0) override;

    virtual bool TryGetUtcWeakupTime(time_t& time) override;

    virtual WakeupReason GetWakeupReason() const override;
    virtual float GetBatteryVoltage() override;
    virtual bool BatteryIsLow() override;

    virtual void SetStatusLED(const bool on) override;

private:
    int GetSleepDuration(const time_t maxUtcWakeTime);
    bool IsBusinessTime(time_t utcNow);

private:
    IAppContext& _ctx;
    WakeupReason _wakeupReason;
};