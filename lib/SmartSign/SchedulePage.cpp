#include "SchedulePage.h"
#include "IAppContext.h"

SchedulePage::SchedulePage(IAppContext& ctx) :
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