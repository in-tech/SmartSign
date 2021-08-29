#pragma once
#include <IAzureClient.h>
#include <map>

class IAppContext;

class AzureClient : public IAzureClient
{
public:
    AzureClient(IAppContext& ctx);
    virtual ~AzureClient() override;

    void Init();
    void Update();

    virtual AuthorizationProgress GetAuthorizationProgress() override;
    virtual String GetAuthorizationError() override;
    virtual AuthorizationRequestInfo BeginAuthorization() override;
    virtual void CancelAuthorization(const String& reason) override;

    virtual GetScheduleResponse GetSchedule(DynamicJsonDocument& json) override;

private:
    Response LoginRequest(const String& endpoint, const std::map<String, String>& bodyData);
    Response ScheduleRequest(const String& accessToken, const String& bodyData);
    String GetErrorMessage(const Response& response);

private:
    IAppContext& _ctx;
    WiFiClientSecure _client;
    AuthorizationProgress _authProg;
    String _lastAuthError;
    String _deviceCode;
};