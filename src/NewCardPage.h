#pragma once
#include "NavPage.h"

class NewCardPage : public NavPage
{
public:
    NewCardPage(AppContext& ctx);
    ~NewCardPage();

    virtual void Update(const InputState& inputState) override;
};