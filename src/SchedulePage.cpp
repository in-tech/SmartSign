#include "SchedulePage.h"
#include "AppContext.h"

//#define USE_MOCK_DATA
//#define SHOW_FONT_TEST_SCREEN

#ifdef USE_MOCK_DATA
RTC_DATA_ATTR int rtc_mock_minute = 0;
#endif

SchedulePage::SchedulePage(AppContext& ctx) :
    NavPage(ctx),
    _scheduleRequested(false),
    _maxUtcWakeTime(0)
{
}

SchedulePage::~SchedulePage()
{    
}

void SchedulePage::Update(const InputState& inputState)
{
#ifdef SHOW_FONT_TEST_SCREEN
    _ctx.GetDisplay().ShowFontTestScreen();
    _ctx.GetPowerMgr().EnterDeepSleep();
    return;
#endif

#ifdef USE_MOCK_DATA
    PresentSchedule("");
#else

    if (!_ctx.GetNetworkMgr().WifiIsConfigured())
    {
        _ctx.GetDisplay().ShowErrorScreen("WiFi not configured!", "", "");
        _ctx.GetPowerMgr().EnterDeepSleep();
        return;
    }

    if (_ctx.GetNetworkMgr().IsPending())
    {
        return;
    }

    if (!_ctx.GetNetworkMgr().WifiIsConnected())
    {
        _ctx.GetDisplay().ShowErrorScreen("WiFi not connected!", "", "Retry");
        _ctx.GetPowerMgr().EnterDeepSleep();
        return;
    }

    if (!_ctx.GetNetworkMgr().TimeIsValid())
    {
        _ctx.GetDisplay().ShowErrorScreen("Time is not synchronized!", "", "Retry");
        _ctx.GetPowerMgr().EnterDeepSleep();
        return;
    }

    if (!_scheduleRequested)
    {
        _scheduleRequested = true;

        DynamicJsonDocument json(16 * 1024);
        auto result = _ctx.GetAzureClient().GetSchedule(json);
        if (result.Success)
        {
            PresentSchedule(json);
        }
        else
        {
            _ctx.GetDisplay().ShowErrorScreen(result.Error, "", "");
        }
    }
#endif

    if (!_ctx.GetDisplay().IsBusy())
    {
        _ctx.GetPowerMgr().EnterDeepSleep(_maxUtcWakeTime);
    }
    else if (inputState.SmartCardPresent)
    {
        Close();
    }
}

void SchedulePage::PresentSchedule(DynamicJsonDocument& json)
{    
    _maxUtcWakeTime = 0;

#ifdef USE_MOCK_DATA
    time_t utcNow = TimeUtils::UtcIsoStringToTime("2021-04-01T09:52:00");
    utcNow += SECS_PER_MIN * rtc_mock_minute;
    rtc_mock_minute += 2;
    time_t localNow = utcNow;

    //Log::Info(TimeUtils::ToShortTimeString(localNow));
    std::vector<ScheduleItem> items;
    {
        ScheduleItem item = {};
        item.Subject = "Heinz, Karl";
        item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T06:30:00");
        item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T06:35:00");
        items.push_back(item);
    }

    {
        ScheduleItem item = {};
        item.Subject = "Wurst, Hans";
        item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T07:30:00");
        item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T10:00:00");
        items.push_back(item);
    }

    {
        ScheduleItem item = {};
        item.Subject = "Mustermann, Max 0123456789ABCDEF";
        item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T10:00:00");
        item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T14:30:00");
        items.push_back(item);
    }

    {
        ScheduleItem item = {};
        item.Subject = "Roßbach, Marc";
        item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T15:00:00");
        item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T16:00:00");
        items.push_back(item);
    }
#else
    std::vector<ScheduleItem> items;

    if (json.containsKey("value") && json["value"].is<JsonArray>())
    {
        auto schedules = json["value"].as<JsonArray>();
        for(auto schedule : schedules)
        {
            if (schedule.containsKey("scheduleItems") && schedule["scheduleItems"].is<JsonArray>())
            {
                auto scheduleItems = schedule["scheduleItems"].as<JsonArray>();
                for (auto scheduleItem : scheduleItems)
                {
                    ScheduleItem item = {};
                    if (scheduleItem.containsKey("subject") && scheduleItem["subject"].is<const char*>())
                    {
                        item.Subject = scheduleItem["subject"].as<const char*>();
                        item.Subject.trim();
                    }

                    if (item.Subject.isEmpty())
                    {
                        item.Subject = "Top Secret!";
                    }

                    if (scheduleItem.containsKey("start") && scheduleItem["start"].is<JsonObject>() &&
                        scheduleItem.containsKey("end") && scheduleItem["end"].is<JsonObject>())
                    {
                        auto start = scheduleItem["start"].as<JsonObject>();
                        auto end = scheduleItem["end"].as<JsonObject>();
                        if (start.containsKey("dateTime") && end.containsKey("dateTime"))
                        {
                            const String utcStartTimeStr = start["dateTime"].as<const char*>();
                            const time_t utcStartTime = TimeUtils::UtcIsoStringToTime(utcStartTimeStr);
                            item.LocalStartTime = TimeUtils::ToLocalTime(utcStartTime);

                            const String utcEndTimeStr = end["dateTime"].as<const char*>();
                            const time_t utcEndTime = TimeUtils::UtcIsoStringToTime(utcEndTimeStr);
                            item.LocalEndTime = TimeUtils::ToLocalTime(utcEndTime);

                            items.push_back(item);
                        }
                    }
                }
            }
        }
    }

    time_t utcNow = 0;
    time_t localNow = 0;
    if (_ctx.GetNetworkMgr().TryGetUtcTime(utcNow))
    {
        localNow = TimeUtils::ToLocalTime(utcNow);

        time_t maxLocalWakeTime = nextMidnight(localNow) + 60; // +60 seconds buffer
        for (auto& item : items)
        {
            if (localNow < item.LocalStartTime)
                maxLocalWakeTime = std::min(maxLocalWakeTime, item.LocalStartTime + 10); // +10 seconds buffer
            if (localNow < item.LocalEndTime)
                maxLocalWakeTime = std::min(maxLocalWakeTime, item.LocalEndTime + 10); // +10 seconds buffer
        }
        _maxUtcWakeTime = TimeUtils::ToUtcTime(maxLocalWakeTime);
    }

#endif

    _ctx.GetPowerMgr().SetStatusLED(false);
    for (auto& item : items)
    {
        if (localNow >= item.LocalStartTime && localNow <= item.LocalEndTime)
        {
            _ctx.GetPowerMgr().SetStatusLED(true);
        }
    }

    _ctx.GetDisplay().ShowScheduleScreen(localNow, items);
}