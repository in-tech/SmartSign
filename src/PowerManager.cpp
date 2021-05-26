#include "PowerManager.h"
#include "AppContext.h"
#include <esp_sleep.h>

#define uS_TO_S_FACTOR 1000000ULL

RTC_DATA_ATTR float rtc_lastBatteryVoltage = 0.0f;
RTC_DATA_ATTR bool rtc_batteryWasLow = false;
RTC_DATA_ATTR bool rtc_lastSleepDurationFailed = false;
RTC_DATA_ATTR time_t rtc_utcWakeupTime = 0;

PowerManager::PowerManager(AppContext& ctx) :
    _ctx(ctx),
    _wakeupReason(WAKEUP_REASON_UNKNWON)
{
}

PowerManager::~PowerManager()
{
}

void PowerManager::Init()
{
    SetStatusLED(false);

    pinMode(VCC_PIN, OUTPUT);
    gpio_set_drive_capability((gpio_num_t)VCC_PIN, GPIO_DRIVE_CAP_3);
    digitalWrite(VCC_PIN, HIGH);
    delay(1);

    _wakeupReason = WAKEUP_REASON_UNKNWON;

    bool wakeupTimeIsValid = false;

    switch (esp_sleep_get_wakeup_cause())
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        {
            _wakeupReason = WAKEUP_REASON_CARD;
            Log::Info("Wakeup caused by external signal using RTC_IO");
        }
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        {
            const uint64_t bitMask = esp_sleep_get_ext1_wakeup_status();
            int pin = 0;
            for (uint64_t i = 0; i < 64; i++)
            {
            if ((bitMask & (1<<i)) != 0)
            {
                pin = i;
                break;
            }
            }
            switch (pin)
            {
                case ENDSTOP_PIN:
                    _wakeupReason = WAKEUP_REASON_CARD;
                    break;
                case BUTTONA_PIN:
                    _wakeupReason = WAKEUP_REASON_BUTTONA;
                    break;
                case BUTTONB_PIN:
                    _wakeupReason = WAKEUP_REASON_BUTTONB;
                    break;
            }
            Log::Info(String("Wakeup caused by external signal using RTC_CNTL: PIN ") + pin);
        }
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        {
            wakeupTimeIsValid = true;
            _wakeupReason = WAKEUP_REASON_TIMER;
            Log::Info("Wakeup caused by timer");
        }
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Log::Info("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        Log::Info("Wakeup caused by ULP program");
        break;
    default:
        Log::Info("Wakeup was not caused by deep sleep");
        break;
    }

    if (!wakeupTimeIsValid)
    {
        rtc_utcWakeupTime = 0;
    }
}

void PowerManager::Update()
{
    if (millis() > 1800000) // 30 min
    {
        EnterDeepSleep();
    }
}

void PowerManager::EnterDeepSleep(const time_t maxUtcWakeTime)
{
    uint64_t mask = 0;
    if (_ctx.GetInputMgr().State().SmartCardPresent)
    {
        // wakeup when card is removed
        esp_sleep_enable_ext0_wakeup((gpio_num_t)ENDSTOP_PIN, 0);
    }
    else
    {
        // wakeup when card is insert
        mask |= 1 << ENDSTOP_PIN;
    }
    mask |= 1 << BUTTONA_PIN;
    mask |= 1 << BUTTONB_PIN;
    esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);

    const int sleepDuration = GetSleepDuration(maxUtcWakeTime);
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * sleepDuration);

    _ctx.DeInit();

    delay(10);

    pinMode(VCC_PIN, OUTPUT);
    digitalWrite(VCC_PIN, LOW);
    pinMode(LED_PIN, INPUT);

    time_t utcNow;
    if (_ctx.GetNetworkMgr().TryGetUtcTime(utcNow))
    {
        const time_t utcWakeTime = utcNow + sleepDuration;
        const time_t localWakeTime = TimeUtils::ToLocalTime(utcWakeTime);
        const time_t localNow = TimeUtils::ToLocalTime(utcNow);
        
        // to prevent large time deviations we clear this at midnight
        if (elapsedDays(localNow) != elapsedDays(localWakeTime))
        {
            rtc_utcWakeupTime = 0;            
        }
        else
        {
            rtc_utcWakeupTime = utcWakeTime;
        }
    }
    else
    {
        rtc_utcWakeupTime = 0;
    }

    Log::Info(String("uptime: ") + millis() + " ms");
    Log::Info(String("going to sleep for ") + sleepDuration + " seconds");
    Serial.flush();

    esp_deep_sleep_start();
}

bool PowerManager::TryGetUtcWeakupTime(time_t& time)
{
    time = rtc_utcWakeupTime;
    return time > 0;
}

WakeupReason PowerManager::GetWakeupReason() const
{
    return _wakeupReason;
}

float PowerManager::GetBatteryVoltage()
{
    pinMode(BAT_LEVEL_PIN, INPUT);
    const uint16_t rawBatLevel = analogRead(BAT_LEVEL_PIN);
    const float batteryVoltage = 2.19f * 3.3f * rawBatLevel / 4095;
    if (rtc_lastBatteryVoltage == 0.0f)
    {
        rtc_lastBatteryVoltage = batteryVoltage;
    }
    else
    {
        rtc_lastBatteryVoltage = 0.9f * rtc_lastBatteryVoltage + 0.1f * batteryVoltage;
    }

    return rtc_lastBatteryVoltage;
}

bool PowerManager::BatteryIsLow()
{
    const float voltage = GetBatteryVoltage();
    if (!rtc_batteryWasLow)
    {
        if (voltage < LOW_BAT_VOLTAGE)
        {
            rtc_batteryWasLow = true;
        }
    }
    else // rtc_batteryWasLow == true
    {
        if (voltage > LOW_BAT_VOLTAGE + 0.1f)
        {
            rtc_batteryWasLow = false;
        }
    }

    return rtc_batteryWasLow;
}

void PowerManager::SetStatusLED(const bool on)
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, on ? HIGH : LOW);    
}

int PowerManager::GetSleepDuration(const time_t maxUtcWakeTime)
{
    int durationInSecs = 0;
    time_t utcNow = 0;
    if (_ctx.GetNetworkMgr().TryGetUtcTime(utcNow))
    {
        rtc_lastSleepDurationFailed = false;
        if (IsBusinessTime(utcNow))
        {
            durationInSecs = _ctx.Settings().BusinessTimeUpdateInterval * SECS_PER_MIN;
        }
        else    
        {
            durationInSecs = _ctx.Settings().FreeTimeUpdateInterval * SECS_PER_MIN;
        }
    }
    else
    {
        // if this is the first time it failed ...
        if (!rtc_lastSleepDurationFailed)
        {
            Log::Warning("time not synchronized");

            rtc_lastSleepDurationFailed = true;

            // ... try again in one minute
            durationInSecs = SECS_PER_MIN;
        }
        else
        {
            Log::Error("time not synchronized");

            // still failing -> sleep for longer
            durationInSecs = _ctx.Settings().FreeTimeUpdateInterval * SECS_PER_MIN;
        }
    }

    if (utcNow > 0 && maxUtcWakeTime > 0 && utcNow + durationInSecs > maxUtcWakeTime)
    {
        if (maxUtcWakeTime > utcNow)
        {
            // reduce sleep duration
            durationInSecs = maxUtcWakeTime - utcNow;
        }
        else
        {
            // we are already too late
            durationInSecs = SECS_PER_MIN;
        }
    }

    return durationInSecs;
}

bool PowerManager::IsBusinessTime(time_t utcNow)
{
    auto& s = _ctx.Settings();
    const time_t localNow = TimeUtils::ToLocalTime(utcNow);
    auto isBusinessDay = [&]()
    {
        String businessDays = s.BusinessDays;
        businessDays.toLowerCase();
        switch (weekday(localNow))
        {
            case dowMonday:
                return businessDays.indexOf("mo") > -1;
            case dowTuesday:
                return businessDays.indexOf("tu") > -1;
            case dowWednesday:
                return businessDays.indexOf("we") > -1;
            case dowThursday:
                return businessDays.indexOf("th") > -1;
            case dowFriday:
                return businessDays.indexOf("fr") > -1;
            case dowSaturday:
                return businessDays.indexOf("sa") > -1;
            case dowSunday:
                return businessDays.indexOf("su") > -1;
        }
        return false;
    };

    if (isBusinessDay())
    {
        return hour(localNow) >= s.BusinessDayBeginHour && hour(localNow) < s.BusinessDayEndHour;
    }

    return false;
}