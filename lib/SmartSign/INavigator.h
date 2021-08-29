#pragma once
#include "NavPage.h"

class INavigator
{
public:
    virtual ~INavigator() {};

    virtual void GoTo(const NavPages page) = 0;
};