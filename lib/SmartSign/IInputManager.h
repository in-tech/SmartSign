#pragma once
#include <Arduino.h>

struct InputState
{
    bool PressedButtonA;
    bool PressedButtonB;
    bool SmartCardPresent;
    String SmartCardId;

    void Reset()
    {
        PressedButtonA = false;
        PressedButtonB = false;
        SmartCardPresent = false;
        SmartCardId.clear();
    }
};

class IInputManager
{
public:
    virtual ~IInputManager() {};
    virtual InputState& State() = 0;
};