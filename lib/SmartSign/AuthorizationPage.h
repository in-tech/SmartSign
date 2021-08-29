#pragma once
#include "NavPage.h"
#include "IAzureClient.h"

class AuthorizationPage : public NavPage
{
public:
    AuthorizationPage(IAppContext& ctx);
    ~AuthorizationPage();

    virtual void Update(const InputState& inputState) override;

private:
    AuthorizationRequestInfo _requestInfo;
    AuthorizationProgress _lastProgress;
};