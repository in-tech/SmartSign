#pragma once
#include <Arduino.h>

enum WakeupReason
{
    WAKEUP_REASON_UNKNWON = 0,
    WAKEUP_REASON_TIMER,
    WAKEUP_REASON_CARD,
    WAKEUP_REASON_BUTTONA,
    WAKEUP_REASON_BUTTONB
};

class IPowerManager
{
public:
    virtual ~IPowerManager() {}

    virtual void EnterDeepSleep(const time_t maxUtcWakeTime = 0) = 0;

    virtual bool TryGetUtcWeakupTime(time_t& time) = 0;

    virtual WakeupReason GetWakeupReason() const = 0;
    virtual float GetBatteryVoltage() = 0;
    virtual bool BatteryIsLow() = 0;

    virtual void SetStatusLED(const bool on) = 0;
};