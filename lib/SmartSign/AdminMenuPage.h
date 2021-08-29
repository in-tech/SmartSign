#pragma once
#include "NavPage.h"

class AdminMenuPage : public NavPage
{
public:
    AdminMenuPage(IAppContext& ctx);
    ~AdminMenuPage();

    virtual void Update(const InputState& inputState) override;
};