#pragma once
#include "NavPage.h"

class SettingsPage : public NavPage
{
public:
    SettingsPage(IAppContext& ctx);
    ~SettingsPage();

    virtual void Update(const InputState& inputState) override;
};