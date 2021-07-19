#pragma once
#include "NavPage.h"

class AdminMenuPage : public NavPage
{
public:
    AdminMenuPage(AppContext& ctx);
    ~AdminMenuPage();

    virtual void Update(const InputState& inputState) override;
};