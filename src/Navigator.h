#pragma once
#include <memory>
#include "NavPage.h"

class AppContext;

class Navigator
{
public:
    Navigator(AppContext& ctx);
    ~Navigator();

    void Init();
    void Update();

    void GoTo(const NavPages page);

private:
    AppContext& _ctx;
    NavPages _currentPage;
    std::unique_ptr<NavPage> _currentPagePtr;
};