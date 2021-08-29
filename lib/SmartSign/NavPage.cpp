#include "NavPage.h"
#include "IAppContext.h"

NavPage::NavPage(IAppContext& ctx) :
    _ctx(ctx)
{
}

NavPage::~NavPage()
{
}

void NavPage::Close()
{
    _ctx.GetNavigator().GoTo(NAV_PAGE_NONE);
}

void NavPage::GoTo(const NavPages page)
{
    _ctx.GetNavigator().GoTo(page);
}