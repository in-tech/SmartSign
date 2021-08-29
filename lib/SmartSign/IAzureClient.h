#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

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

class IAzureClient
{
public:
    virtual ~IAzureClient() {};

    virtual AuthorizationProgress GetAuthorizationProgress() = 0;
    virtual String GetAuthorizationError() = 0;
    virtual AuthorizationRequestInfo BeginAuthorization() = 0;
    virtual void CancelAuthorization(const String& reason) = 0;

    virtual GetScheduleResponse GetSchedule(DynamicJsonDocument& json) = 0;
};