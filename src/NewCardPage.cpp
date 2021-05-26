#include "NewCardPage.h"
#include "AppContext.h"

NewCardPage::NewCardPage(AppContext& ctx) :
    NavPage(ctx)
{
}

NewCardPage::~NewCardPage()
{    
}

void NewCardPage::Update(const InputState& inputState)
{
    const String cardId = _ctx.GetInputMgr().State().SmartCardId;
    _ctx.GetDisplay().ShowUnknownCardScreen("Unknown Smart-Card", cardId, "", "");
    _ctx.GetPowerMgr().SetStatusLED((millis() / 500) % 2 == 0);

    if (!inputState.SmartCardPresent)
    {
        _ctx.GetPowerMgr().SetStatusLED(false);
        Close();
    }
}