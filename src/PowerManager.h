#pragma once
#include <Arduino.h>
#include "Config.h"

enum WakeupReason
{
    WAKEUP_REASON_UNKNWON = 0,
    WAKEUP_REASON_TIMER,
    WAKEUP_REASON_CARD,
    WAKEUP_REASON_BUTTONA,
    WAKEUP_REASON_BUTTONB
};

class AppContext;

class PowerManager
{
public:
    PowerManager(AppContext& ctx);
    ~PowerManager();

    void Init();
    void Update();
    void EnterDeepSleep(const time_t maxUtcWakeTime = 0);

    bool TryGetUtcWeakupTime(time_t& time);

    WakeupReason GetWakeupReason() const;
    float GetBatteryVoltage();
    bool BatteryIsLow();

    void SetStatusLED(const bool on);

private:
    int GetSleepDuration(const time_t maxUtcWakeTime);
    bool IsBusinessTime(time_t utcNow);

private:
    AppContext& _ctx;
    WakeupReason _wakeupReason;
};