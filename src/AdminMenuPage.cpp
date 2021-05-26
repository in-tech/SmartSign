#include "AdminMenuPage.h"
#include "AppContext.h"

AdminMenuPage::AdminMenuPage(AppContext& ctx) :
    NavPage(ctx)
{
}

AdminMenuPage::~AdminMenuPage()
{    
}

void AdminMenuPage::Update(const InputState& inputState)
{
    if (_ctx.HasPermissions(PERM_ADMINISTRATION))
    {
        _ctx.GetPowerMgr().SetStatusLED((millis() / 500) % 2 == 0);
        _ctx.GetDisplay().ShowAdminMenuScreen("Authorization", "Setup");

        if (inputState.PressedButtonA)
        {
            GoTo(NAV_PAGE_AUTHORIZATION);
        }
        else if (inputState.PressedButtonB)
        {
            GoTo(NAV_PAGE_ADMIN_SETTINGS);
        }
    }
    else
    {
        _ctx.GetPowerMgr().SetStatusLED(false);
        Close();
    }
}