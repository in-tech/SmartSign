#pragma once
#include "IInputManager.h"
#include "CardReader.h"

class IAppContext;

class InputManager : public IInputManager
{
public:
    InputManager(IAppContext& ctx);
    ~InputManager();

    void Init();
    void Update();

    virtual InputState& State() override;

private:
    IAppContext& _ctx;
    CardReader _cardReader;
    InputState _state;
    bool _resetState;
};