#pragma once
#include <Arduino.h>
#include "CardReader.h"

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

class AppContext;

class InputManager
{
public:
    InputManager(AppContext& ctx);
    ~InputManager();

    void Init();
    void Update();

    InputState& State();

private:
    AppContext& _ctx;
    CardReader _cardReader;
    InputState _state;
    bool _resetState;
};