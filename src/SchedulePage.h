#pragma once
#include "NavPage.h"
#include <TimeUtils.h>
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
    SchedulePage(AppContext& ctx);
    ~SchedulePage();

    virtual void Update(const InputState& inputState) override;
    void PresentSchedule(const String& jsonContent);

private:
    bool _scheduleRequested;
    time_t _maxUtcWakeTime;
};