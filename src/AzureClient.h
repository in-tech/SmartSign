#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <map>

class AppContext;

struct Response
{
    t_http_codes HttpCode;
    String Content;

    Response(const t_http_codes httpCode, const String& content) :
        HttpCode(httpCode),
        Content(content)
    {        
    }
};

struct AuthorizationRequestInfo
{
    bool Success;
    String Error;
    String UserCode;
    String DeviceCode;
    String Message;
    String Uri;
};

struct GetScheduleResponse
{
    bool Success;
    String Error;
};

enum AuthorizationProgress
{
    AUTH_PROG_UNKNOWN = 0,
    AUTH_PROG_PENDING,
    AUTH_PROG_COMPLETED,
    AUTH_PROG_FAILED_OR_CANCELED
};

class AzureClient
{
public:
    AzureClient(AppContext& ctx);
    ~AzureClient();

    void Init();
    void Update();

    AuthorizationProgress GetAuthorizationProgress();
    String GetAuthorizationError();
    AuthorizationRequestInfo BeginAuthorization();
    void CancelAuthorization(const String& reason);

    GetScheduleResponse GetSchedule(DynamicJsonDocument& json);

private:
    Response LoginRequest(const String& endpoint, const std::map<String, String>& bodyData);
    Response ScheduleRequest(const String& accessToken, const String& bodyData);
    String GetErrorMessage(const Response& response);

private:
    AppContext& _ctx;
    WiFiClientSecure _client;
    AuthorizationProgress _authProg;
    String _lastAuthError;
    String _deviceCode;
};