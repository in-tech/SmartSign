#pragma once
#include "NavPage.h"
#include "AzureClient.h"

class AuthorizationPage : public NavPage
{
public:
    AuthorizationPage(AppContext& ctx);
    ~AuthorizationPage();

    virtual void Update(const InputState& inputState) override;

private:
    AuthorizationRequestInfo _requestInfo;
    AuthorizationProgress _lastProgress;
};