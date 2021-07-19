#include "Navigator.h"
#include "AppContext.h"
#include "SchedulePage.h"
#include "NewCardPage.h"
#include "AdminMenuPage.h"
#include "SettingsPage.h"
#include "AuthorizationPage.h"

Navigator::Navigator(AppContext& ctx) :
    _ctx(ctx),
    _currentPage(NAV_PAGE_NONE)
{
}

Navigator::~Navigator()
{    
}

void Navigator::Init()
{
    GoTo(NAV_PAGE_NONE);
}

void Navigator::Update()
{
    const auto inputState = _ctx.GetInputMgr().State();
    if (_currentPage == NAV_PAGE_NONE)
    {
        if (!inputState.SmartCardId.isEmpty())
        {
            if (_ctx.HasPermissions(PERM_ADMINISTRATION))
            {
                GoTo(NAV_PAGE_ADMIN_MENU);
            }
            else if (_ctx.HasPermissions(PERM_AUTHORIZATION))
            {
                GoTo(NAV_PAGE_AUTHORIZATION);
            }
            else
            {
                GoTo(NAV_PAGE_NEW_SMART_CARD);
            }
        }
        else
        {
            GoTo(NAV_PAGE_SCHEDULE);
        }
    }

    if (_currentPagePtr != nullptr)
    {
        _currentPagePtr->Update(inputState);
    }
}

void Navigator::GoTo(const NavPages page)
{
    if (page != _currentPage)
    {
        Log::Info(String("goto page: ") + (int)page);
        switch (page)
        {
            case NAV_PAGE_NONE:
                _currentPagePtr = nullptr;                
                break;
            case NAV_PAGE_SCHEDULE:
                _currentPagePtr = std::unique_ptr<NavPage>(new SchedulePage(_ctx));
                break;
            case NAV_PAGE_NEW_SMART_CARD:
                _currentPagePtr = std::unique_ptr<NavPage>(new NewCardPage(_ctx));
                break;
            case NAV_PAGE_ADMIN_MENU:
                _currentPagePtr = std::unique_ptr<NavPage>(new AdminMenuPage(_ctx));
                break;
            case NAV_PAGE_ADMIN_SETTINGS:
                _currentPagePtr = std::unique_ptr<NavPage>(new SettingsPage(_ctx));
                break;
            case NAV_PAGE_AUTHORIZATION:
                _currentPagePtr = std::unique_ptr<NavPage>(new AuthorizationPage(_ctx));
                break;
        }

        _currentPage = page;
    }
}