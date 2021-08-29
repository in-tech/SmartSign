#pragma once
#include "NavPage.h"
#include <TimeUtils.h>
#include <ArduinoJson.h>
#include <vector>

struct ScheduleItem
{
    time_t LocalStartTime;
    time_t LocalEndTime;
    String Subject;
};

class SchedulePage : public NavPage
{
public:
    SchedulePage(IAppContext& ctx);
    ~SchedulePage();

    virtual void Update(const InputState& inputState) override;
    void PresentSchedule(DynamicJsonDocument& json);

private:
    bool _scheduleRequested;
    time_t _maxUtcWakeTime;
};