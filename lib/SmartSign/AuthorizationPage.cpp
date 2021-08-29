#include "AuthorizationPage.h"
#include "IAppContext.h"

AuthorizationPage::AuthorizationPage(IAppContext& ctx) :
    NavPage(ctx), _lastProgress(AUTH_PROG_UNKNOWN)
{
    _ctx.GetDisplay().ShowInfoScreen("Please wait ...", "", "");
    _requestInfo = ctx.GetAzureClient().BeginAuthorization();
}

AuthorizationPage::~AuthorizationPage()
{    
}

void AuthorizationPage::Update(const InputState& inputState)
{
    auto& ac = _ctx.GetAzureClient();
    auto& pw = _ctx.GetPowerMgr();
    auto& ed = _ctx.GetDisplay();

    pw.SetStatusLED((millis() / 500) % 2 == 0);
    
    const bool isAdmin = _ctx.HasPermissions(PERM_ADMINISTRATION);
    const String backBtn = isAdmin ? "Back" : "";
    if (!_ctx.HasPermissions(PERM_AUTHORIZATION) || (isAdmin && inputState.PressedButtonB))
    {
        ac.CancelAuthorization("cancelled by user");
        pw.SetStatusLED(false);

        if (isAdmin)
        {
            GoTo(NAV_PAGE_ADMIN_MENU);
        }
        else
        {
            GoTo(NAV_PAGE_SCHEDULE);
        }
    }
    else
    {
        auto progress = ac.GetAuthorizationProgress();
        if (_lastProgress != progress)
        {
            _lastProgress = progress;
            if (progress == AUTH_PROG_PENDING)
            {
                if (_requestInfo.Success)
                {
                    ed.ShowAuthorizationScreen(
                        _requestInfo.Message,
                        _requestInfo.Uri,
                        _requestInfo.UserCode, "", backBtn);
                }
                else
                {
                    ed.ShowErrorScreen(_requestInfo.Error, "", backBtn);
                }
            }
            else if (progress == AUTH_PROG_COMPLETED)
            {
                ed.ShowInfoScreen("Authorization Completed!", "", backBtn);
            }
            else
            {
                ed.ShowErrorScreen(ac.GetAuthorizationError(), "", backBtn);
            }
        }
    }
}