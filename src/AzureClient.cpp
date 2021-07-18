#include "AzureClient.h"
#include "AppContext.h"
#include "TimeUtils.h"
#include "AzureToken.hpp"
#include "Certificates.h"

#define NUM_RETRIES 1
#define MAX_ACCESS_TOKEN_AGE (SECS_PER_MIN * 50)

RTC_DATA_ATTR AzureToken<1080> rtc_refreshToken = {};

RTC_DATA_ATTR time_t rtc_accessTokenTimeStamp = 0;
RTC_DATA_ATTR AzureToken<2210> rtc_accessToken = {};

void ClearAccessToken()
{
    rtc_accessTokenTimeStamp = 0;
    rtc_accessToken.Valid = false;
}

bool StoreAccessToken(const time_t utcNow, const String& token)
{
    rtc_accessTokenTimeStamp = utcNow;
    return rtc_accessToken.Set(token);
}

String LoadAccessToken(const time_t utcNow)
{
    if (rtc_accessToken.Valid && rtc_accessTokenTimeStamp + MAX_ACCESS_TOKEN_AGE > utcNow)
    {
        return rtc_accessToken.Get();
    }
    return "";
}

AzureClient::AzureClient(AppContext &ctx) : _ctx(ctx), _authProg(AUTH_PROG_UNKNOWN)
{
}

AzureClient::~AzureClient()
{
}

void AzureClient::Init()
{
}

void AzureClient::Update()
{
    if (!_ctx.HasPermissions(PERM_AUTHORIZATION))
    {
        CancelAuthorization("invalid smart-card");
    }

    if (!_deviceCode.isEmpty() && (millis() / 1000) % 2 == 0)
    {
        std::map<String, String> body;
        body["client_id"] = _ctx.Settings().AzureClientId;
        body["grant_type"] = "urn:ietf:params:oauth:grant-type:device_code";
        body["device_code"] = _deviceCode;
        auto response = LoginRequest("token", body);
        {
            DynamicJsonDocument json(4096);
            if (deserializeJson(json, response.Content) == DeserializationError::Ok)
            {
                if (response.HttpCode == HTTP_CODE_OK)
                {
                    if (json.containsKey("refresh_token"))
                    {
                        rtc_refreshToken.Set(json["refresh_token"].as<const char *>());

                        ClearAccessToken();

                        _deviceCode.clear();
                        _authProg = AUTH_PROG_COMPLETED;                        
                        Log::Info("authorization completed");
                        return;
                    }
                }
                else if (json.containsKey("error"))
                {
                    if (String("authorization_pending") == json["error"].as<const char *>())
                    {
                        // this error is fine 
                        Log::Error(json["error"].as<const char *>());
                        return;
                    }
                }
            }
        }

        // something went wrong
        CancelAuthorization(GetErrorMessage(response));
    }
}

AuthorizationProgress AzureClient::GetAuthorizationProgress()
{
    return _authProg;
}

String AzureClient::GetAuthorizationError()
{
    return _lastAuthError;
}

AuthorizationRequestInfo AzureClient::BeginAuthorization()
{
    AuthorizationRequestInfo result = {};

    std::map<String, String> body;
    body["client_id"] = _ctx.Settings().AzureClientId;
    body["scope"] = "calendars.read+calendars.read.shared+offline_access";
    auto response = LoginRequest("devicecode", body);

    if (response.HttpCode == HTTP_CODE_OK)
    {
        DynamicJsonDocument json(1024);
        if (deserializeJson(json, response.Content) == DeserializationError::Ok)
        {
            if (json.containsKey("user_code"))
            {
                result.UserCode = json["user_code"].as<const char *>();
            }
            if (json.containsKey("device_code"))
            {
                result.DeviceCode = json["device_code"].as<const char *>();
            }
            if (json.containsKey("verification_uri"))
            {
                result.Uri = json["verification_uri"].as<const char *>();
            }
            if (json.containsKey("message"))
            {
                result.Message = json["message"].as<const char *>();
                Log::Info(result.Message);
            }
        }
    }
    else
    {
        result.Error = GetErrorMessage(response);
    }

    result.Success = !result.UserCode.isEmpty() && !result.DeviceCode.isEmpty();

    if (result.Success)
    {
        _authProg = AUTH_PROG_PENDING;
        _lastAuthError.clear();
        _deviceCode = result.DeviceCode;
    }
    else
    {
        _authProg = AUTH_PROG_FAILED_OR_CANCELED;
        _lastAuthError = result.Error;
        _deviceCode.clear();
        Log::Error(_lastAuthError);
    }

    return result;
}

void AzureClient::CancelAuthorization(const String &reason)
{
    if (!_deviceCode.isEmpty())
    {
        Log::Warning(String("authorization aborted: ") + reason);
        _deviceCode.clear();
        _authProg = AUTH_PROG_FAILED_OR_CANCELED;
        _lastAuthError = reason;
    }
}

Response AzureClient::LoginRequest(const String &endpoint, const std::map<String, String>& bodyData)
{
    if (_ctx.GetNetworkMgr().WifiIsConnected())
    {
        for (int i = 0; i <= NUM_RETRIES; i++)
        {
            _client.setCACert(DigiCertGlobalRootCA);
            const String url = String("https://login.microsoftonline.com/") + _ctx.Settings().AzureTenantId + "/oauth2/v2.0/" + endpoint;
            HTTPClient http;
            http.setTimeout(TCP_TIMEOUT);
            if (http.begin(_client, url))
            {
                http.addHeader("Content-Type", "application/x-www-form-urlencoded");
                String body;
                for (auto &entry : bodyData)
                {
                    if (!body.isEmpty())
                    {
                        body += "&";
                    }
                    body += entry.first;
                    body += "=";
                    body += entry.second;
                }
                Log::Info(String("Request: ") + url);
                const int httpCode = http.POST(body);
                Log::Info(String("ResponseCode: ") + httpCode);
                if (httpCode < 0)
                {
                    if (i < NUM_RETRIES)
                    {
                        delay(10);                        
                        Log::Warning(HTTPClient::errorToString(httpCode) + " -> retry ...");
                        continue;
                    }
                    return Response(HTTP_CODE_SERVICE_UNAVAILABLE, HTTPClient::errorToString(httpCode));
                }
                return Response((t_http_codes)httpCode, http.getString());
            }
        }
    }
    return Response(HTTP_CODE_SERVICE_UNAVAILABLE, "not connected");
}

GetScheduleResponse AzureClient::GetSchedule(DynamicJsonDocument& json)
{
    GetScheduleResponse result = {};

    time_t utcNow = 0;
    if (!_ctx.GetNetworkMgr().TryGetUtcTime(utcNow))
    {
        result.Error = "time not synchronized";
        return result;       
    }

    String referehToken = rtc_refreshToken.Get();
    if (referehToken.isEmpty())
    {
        result.Error = "authorization expired";
        return result;
    }

    // try to re-use the old access token
    String accessToken = LoadAccessToken(utcNow);

    for (int i = 0; i <= NUM_RETRIES; i++)
    {
        if (i > 0)
        {
            Log::Warning(result.Error + " -> retry ...");
        }

        json.clear();
        result.Error = "";

        // use refersh token to get new access token
        if (accessToken.isEmpty())
        {
            std::map<String, String> body;
            body["client_id"] = _ctx.Settings().AzureClientId;
            body["grant_type"] = "refresh_token";
            body["scope"] = "calendars.read calendars.read.shared offline_access";
            body["refresh_token"] = referehToken;
            auto response = LoginRequest("token", body);
            
            if (response.HttpCode == HTTP_CODE_OK)
            {
                DynamicJsonDocument json(4096);
                if (deserializeJson(json, response.Content) == DeserializationError::Ok)
                {
                    if (json.containsKey("refresh_token"))
                    {
                        rtc_refreshToken.Set(json["refresh_token"].as<const char *>());
                    }
                    if (json.containsKey("access_token"))
                    {
                        accessToken = json["access_token"].as<const char *>();

                        // store in rtc memory
                        StoreAccessToken(utcNow, accessToken);
                    }
                }
            }
            else
            {
                result.Error = GetErrorMessage(response);
                return result;
            }
        }

        if (accessToken.isEmpty())
        {
            result.Error = "invalid access-token";
            return result;
        }

        // request schedule data
        {
            String body = R"({
                schedules: ["<<RES>>"],
                startTime: {
                    dateTime: "<<START>>",
                    timeZone: "UTC",
                },
                endTime: {
                    dateTime: "<<END>>",
                    timeZone: "UTC",
                },
                availabilityViewInterval: 60, })";

            const time_t localNow = TimeUtils::ToLocalTime(utcNow);
            const time_t utcStart = TimeUtils::ToUtcTime(previousMidnight(localNow));
            const time_t utcEnd = TimeUtils::ToUtcTime(nextMidnight(localNow));

            const String startDateTimeUTC = TimeUtils::ToIsoString(utcStart);
            const String endDateTimeUTC = TimeUtils::ToIsoString(utcEnd);

            body.replace("<<RES>>", _ctx.Settings().Schedule);
            body.replace("<<START>>", startDateTimeUTC);
            body.replace("<<END>>", endDateTimeUTC);

            auto response = ScheduleRequest(accessToken, body);

            if (response.HttpCode != HTTP_CODE_OK)
            {
                ClearAccessToken();
                accessToken = "";
                result.Error = GetErrorMessage(response);
                continue;
            }
            else
            {
                if (deserializeJson(json, response.Content) != DeserializationError::Ok)
                {
                    result.Error = "Failed to parse schedule data!";
                    continue;
                }
                result.Success = true;
                return result;
            }
        }
    }
    return result;
}

Response AzureClient::ScheduleRequest(const String& accessToken, const String& bodyData)
{
    if (_ctx.GetNetworkMgr().WifiIsConnected())
    {
        for (int i = 0; i <= NUM_RETRIES; i++)
        {
            _client.setCACert(DigiCertGlobalRootG2);
            const String url("https://graph.microsoft.com/v1.0/me/calendar/getSchedule");
            HTTPClient http;
            http.setTimeout(TCP_TIMEOUT);
            if (http.begin(_client, url))
            {
                http.addHeader("Content-Type", "application/json");
                http.addHeader("Authorization", String("Bearer ") + accessToken);
                Log::Info(String("Request: ") + url);
                const int httpCode = http.POST(bodyData);
                Log::Info(String("ResponseCode: ") + httpCode);
                if (httpCode < 0)
                {
                    if (i < NUM_RETRIES)
                    {
                        delay(10);
                        Log::Warning(HTTPClient::errorToString(httpCode) + " -> retry ...");
                        continue;
                    }
                    return Response(HTTP_CODE_SERVICE_UNAVAILABLE, HTTPClient::errorToString(httpCode));
                }
                return Response((t_http_codes)httpCode, http.getString());
            }
        }
    }
    return Response(HTTP_CODE_SERVICE_UNAVAILABLE, "not connected");
}

String AzureClient::GetErrorMessage(const Response& response)
{
    if (response.HttpCode != HTTP_CODE_OK)
    {
        DynamicJsonDocument json(4096);
        if (deserializeJson(json, response.Content) == DeserializationError::Ok)
        {
            if (json.containsKey("error"))
            {
                if (json["error"].is<JsonObject>())
                {
                    auto error = json["error"].as<JsonObject>();
                    if (error.containsKey("message"))
                    {
                        return error["message"].as<const char *>();
                    }
                }
                else if (json["error"].is<const char*>())
                {
                    String error = json["error"].as<const char *>();
                    if (json.containsKey("error_uri"))
                    {
                        error += String(" ") + json["error_uri"].as<const char *>();
                    }
                    return error;
                }
            }
        }
        else
        {
            return response.Content;
        }
    }
    return "";
}