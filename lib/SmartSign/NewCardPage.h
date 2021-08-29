#pragma once
#include "NavPage.h"

class NewCardPage : public NavPage
{
public:
    NewCardPage(IAppContext& ctx);
    ~NewCardPage();

    virtual void Update(const InputState& inputState) override;
};