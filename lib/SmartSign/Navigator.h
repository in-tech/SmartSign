#pragma once
#include <memory>
#include "INavigator.h"

class IAppContext;

class Navigator : public INavigator
{
public:
    Navigator(IAppContext& ctx);
    virtual ~Navigator() override;

    void Init();
    void Update();

    virtual void GoTo(const NavPages page) override;

private:
    IAppContext& _ctx;
    NavPages _currentPage;
    std::unique_ptr<NavPage> _currentPagePtr;
};