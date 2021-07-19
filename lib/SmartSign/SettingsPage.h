#pragma once
#include "NavPage.h"

class SettingsPage : public NavPage
{
public:
    SettingsPage(AppContext& ctx);
    ~SettingsPage();

    virtual void Update(const InputState& inputState) override;
};