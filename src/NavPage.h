#pragma once
#include <Arduino.h>
#include "InputManager.h"

class AppContext;

enum NavPages
{
    NAV_PAGE_NONE = 0,
    NAV_PAGE_SCHEDULE,
    NAV_PAGE_NEW_SMART_CARD,
    NAV_PAGE_ADMIN_MENU,
    NAV_PAGE_ADMIN_SETTINGS,
    NAV_PAGE_AUTHORIZATION
};

class NavPage
{
public:
    NavPage(AppContext& ctx);
    virtual ~NavPage();

    virtual void Update(const InputState& inputState) = 0;

    virtual void Close();
    virtual void GoTo(const NavPages page);

protected:
    AppContext& _ctx;
};