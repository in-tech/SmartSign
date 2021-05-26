#include "InputManager.h"
#include "AppContext.h"

volatile unsigned long lastButtonPressed = -1;
volatile bool pressedButtonA = false;
void IRAM_ATTR ButtonA_ISR()
{
    auto now = millis();
    if (lastButtonPressed > now || (now - lastButtonPressed) > 500)
    {
        pressedButtonA = true;
        lastButtonPressed = now;
    }
}

volatile bool pressedButtonB = false;
void IRAM_ATTR ButtonB_ISR()
{
    auto now = millis();
    if (lastButtonPressed > now || (now - lastButtonPressed) > 500)
    {
         pressedButtonB = true;
        lastButtonPressed = now;
    }   
}

InputManager::InputManager(AppContext& ctx) :
    _ctx(ctx), _resetState(true)
{
    _state.Reset();
}

InputManager::~InputManager()
{    
}

void InputManager::Init()
{
    _cardReader.Init();

    const WakeupReason r = _ctx.GetPowerMgr().GetWakeupReason();
    pressedButtonA = (r == WAKEUP_REASON_BUTTONA);
    pressedButtonB = (r == WAKEUP_REASON_BUTTONB);

    pinMode(BUTTONA_PIN, INPUT);
    attachInterrupt(BUTTONA_PIN, ButtonA_ISR, RISING);

    pinMode(BUTTONB_PIN, INPUT);
    attachInterrupt(BUTTONB_PIN, ButtonB_ISR, RISING);
}

void InputManager::Update()
{
    if (_resetState)
    {
        _resetState = false;
        _state.Reset();
    }
    _state.PressedButtonA |= pressedButtonA;
    pressedButtonA = false;
    _state.PressedButtonB |= pressedButtonB;
    pressedButtonB = false;
    _state.SmartCardId = _cardReader.ReadCardId();
    _state.SmartCardPresent = _cardReader.CardIsPresent();

    if (_state.PressedButtonA)
        Log::Info("Pressed A");

    if (_state.PressedButtonB)
        Log::Info("Pressed B");
}

InputState& InputManager::State()
{
    _resetState = true;
    return _state;
}