#include "Display.h"
#include "IAppContext.h"
#include <images.h>
#include <fonts.h>
#include "TimeUtils.h"
#include "CryptoUtils.h"
#include <qrcode.h>
#include "Log.h"

extern sIMAGE IMG_background;
extern sIMAGE IMG_booked;
extern sIMAGE IMG_free;
extern sIMAGE IMG_cwa;

// Hash of the last content that was presented.
// The hash is stored in RTC memory to survive deep-sleep.
#define MAX_HASH_SIZE 65
RTC_DATA_ATTR char rtc_lastHash[MAX_HASH_SIZE] = "";

Display::Display(IAppContext& ctx) :
    _ctx(ctx),
    _frameBuffer(EPD_WIDTH * EPD_HEIGHT / 4),
    _paint(_frameBuffer.data(), EPD_WIDTH, EPD_HEIGHT, TWO_BITS),
    _active(false)
{
}

Display::~Display()
{
}

void Display::Init()
{
    SPI.begin();
    pinMode(EPD_CS_PIN, OUTPUT);
    digitalWrite(EPD_CS_PIN, HIGH);
}

void Display::DeInit()
{
    if (_active)
    {
        _epd.WaitUntilIdle();
        _epd.Sleep();
    }
}

void Display::ShowScheduleScreen(const time_t localNow, const std::vector<ScheduleItem>& items)
{
    const String date = TimeUtils::ToDateString(localNow);
    const String name = _ctx.Settings().DisplayName;
    const bool lowBattery = _ctx.GetPowerMgr().BatteryIsLow();

    String itemsCacheStr;
    bool booked = false;
    time_t until = 0;
    std::vector<ScheduleItem> upcomingItems;
    for (auto& item : items)
    {
        itemsCacheStr += item.Subject;
        itemsCacheStr += item.LocalStartTime;
        itemsCacheStr += item.LocalEndTime;
        itemsCacheStr += (localNow >= item.LocalStartTime) ? "[" : "";
        itemsCacheStr += (localNow < item.LocalEndTime) ? "]" : "";

        if (booked)
        {
            if (until >= item.LocalStartTime)
            {
                until = item.LocalEndTime;
            }
        }
        else
        {
            if (until == 0 && localNow < item.LocalStartTime)
            {
                until = item.LocalStartTime;
            }
        }

        if (localNow < item.LocalEndTime)
        {
            upcomingItems.push_back(item);
            if (localNow >= item.LocalStartTime)
            {
                booked = true;
                until = item.LocalEndTime;
            }
        }
    }

    if (PrepareHash("ShowScheduleScreen: " + date + name + itemsCacheStr + booked + until + String(lowBattery)))
    {
        // fonts
        sFONT* font = &Overpass_mono28;
        auto drawText = [&](const int x, const int y, const String& txt, const int color, const TextAlignment alignment = TextAlignment::LEFT)
        {
            _paint.DrawUtf8StringAt(x, y - 7, txt.c_str(), font, color, alignment);
        };

        // background
        _paint.DrawImage(0, 0, &IMG_background);

        // header    
        drawText(20, 26, date, WHITE);
        drawText(IMG_background.Width - 20, 26, name, WHITE, TextAlignment::RIGHT);

        // label
        sIMAGE* labelImg = &IMG_free;
        int labelColor = BLACK;
        if (booked)
        {
            labelImg = &IMG_booked;
            labelColor = RED;
        }
        _paint.DrawImage(428 - labelImg->Width / 2, 97, labelImg, labelColor, WHITE);

        // until
        if (until > 0)
        {
            const String untilStr = String("until ") + TimeUtils::ToShortTimeString(until);
            drawText(428, 170, untilStr, (booked ? RED : BLACK), TextAlignment::CENTER);
        }

        // upcoming
        for (int i = 0; i < std::min<int>(2, upcomingItems.size()); i++)
        {
            const int yOffset = i * 84;
            const ScheduleItem& item = upcomingItems[i];
            int color = BLACK;
            if (localNow >= item.LocalStartTime && localNow < item.LocalEndTime)
            {
                // white font
                color = WHITE;

                // red background                
                _paint.DrawFilledRectangle(225, 207 + yOffset, IMG_background.Width - 9, 289 + yOffset, RED);
            }
            drawText(240, 226 + yOffset, TimeUtils::ToShortTimeString(item.LocalStartTime), color);
            drawText(240, 255 + yOffset, TimeUtils::ToShortTimeString(item.LocalEndTime), color);
            drawText(320, 233 + yOffset, item.Subject.substring(0, 25), color);
        }

        // CWA QR-Code
        if (booked && !upcomingItems.empty() && _ctx.Settings().CwaEventQRCodes)
        {
            _paint.DrawFilledRectangle(0, 69, 213, EPD_HEIGHT, WHITE);

            RenderCwaEventQRCode(*upcomingItems.begin());            
        }
        else
        {
            // timebars
            auto drawTimebars = [&](const int x, const time_t tmin, const time_t tmax)
            {
                const int yOffset = 69;
                const time_t timeRange = tmax - tmin;
                const int pixRange = IMG_background.Height - yOffset;
                for (auto& item : items)
                {
                    if (item.LocalStartTime < tmax && item.LocalEndTime > tmin)
                    {
                        const time_t start = max(item.LocalStartTime, tmin);
                        const time_t end = min(item.LocalEndTime, tmax);
                        const int barStart = yOffset + pixRange * (start - tmin) / timeRange;
                        const int barHeight = pixRange * (end - start) / timeRange;
                        _paint.DrawFilledRectangle(x, barStart, x + 40, barStart + barHeight, RED);
                    }
                }
            };
            
            time_t midnight = previousMidnight(localNow);
            drawTimebars(53, midnight + SECS_PER_HOUR * 5 + SECS_PER_MIN * 30, midnight + SECS_PER_HOUR * 12 + SECS_PER_MIN * 30);
            drawTimebars(163, midnight + SECS_PER_HOUR * 12 + SECS_PER_MIN * 30, midnight + SECS_PER_HOUR * 19 + SECS_PER_MIN * 30);
        }

        if (lowBattery)
        {
            _paint.DrawImage(IMG_background.Width - 48, IMG_background.Height - 25, &IMG_bat_0);
        }

        Present(false);
    }
}

void Display::ShowAdminMenuScreen(const String& btnInfoA, const String& btnInfoB)
{
    std::vector<String> lines;
    lines.push_back(String("Battery  : ") + String(_ctx.GetPowerMgr().GetBatteryVoltage(), 1) + "V");
    String wifi = _ctx.Settings().WifiSSID;
    if (wifi.isEmpty())
    {
        wifi += "not configured";
    }
    else if (_ctx.GetNetworkMgr().IsPending())
    {
        wifi += " connecting...";
    }
    else if (!_ctx.GetNetworkMgr().WifiIsConnected())
    {
        wifi += " disconnected";
    }
    lines.push_back(String("WiFi     : ") + wifi);

    time_t utcNow;
    if (_ctx.GetNetworkMgr().TryGetUtcTime(utcNow))
    {
        time_t localNow = TimeUtils::ToLocalTime(utcNow);
        lines.push_back(String("DateTime : ") + TimeUtils::ToDateString(localNow) + " " + TimeUtils::ToShortTimeString(localNow));
    }
    else
    {
        lines.push_back("DateTime : N/A");
    }

    int maxWidth = 0;
    String content;
    for (auto& line : lines)
    {
        content += line;
        content += "$";
        maxWidth = std::max<int>(maxWidth, line.length());
    }

    if (PrepareHash("ShowAdminMenuScreen: " + content + btnInfoA + btnInfoB))
    {
        _paint.Clear(WHITE);
        const int yPos = EPD_HEIGHT / 2 - lines.size() * 11;
        for (int i = 0; i < lines.size(); i++)
        {
            _paint.DrawUtf8StringAt(200, yPos + i * 22, lines[i].c_str(), &Font16, BLACK);
        }

        RenderButtonInfos(btnInfoA, btnInfoB, BLACK);

        RenderFirmwareVersion();
        
        Present(true);
    }
}

void Display::ShowSettingsScreen(const String& wifiSsid, const String& wifiKey, const String& btnInfoA, const String& btnInfoB)
{
    const String content = wifiSsid + wifiKey + btnInfoA + btnInfoB;

    if (PrepareHash("ShowSettingsScreen: " + content))
    {
        _paint.Clear(WHITE);

        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, 50, "Connect to this WiFi to access the settings screen.", &Font16, BLACK, TextAlignment::CENTER, 31);

        const String code = String("WIFI:T:WPA;S:") + wifiSsid + ";P:" + wifiKey + ";;";;
        RenderQRCode(code, EPD_WIDTH / 2 - 87, EPD_HEIGHT / 2 - 87, 174);

        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT - 80, (String("SSID: ") + wifiSsid).c_str(), &Font16, BLACK, TextAlignment::CENTER);
        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT - 70 + Font16.Height, (String("Key: ") + wifiKey).c_str(), &Font16, BLACK, TextAlignment::CENTER);

        RenderButtonInfos(btnInfoA, btnInfoB, BLACK);

        Present(true);
    }
}

void Display::ShowAuthorizationScreen(const String& message, const String& uri, const String& code, const String& btnInfoA, const String& btnInfoB)
{
    const String content = message + uri + code + btnInfoA + btnInfoB;

    if (PrepareHash("ShowAuthorizationScreen: " + content))
    {
        _paint.Clear(WHITE);
        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, 30, message.c_str(), &Font16, BLACK, TextAlignment::CENTER, 47);

        RenderQRCode(uri, EPD_WIDTH / 2 - 87, EPD_HEIGHT / 2 - 87, 174);

        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT - 74, code.c_str(), &Font16, BLACK, TextAlignment::CENTER);

        RenderButtonInfos(btnInfoA, btnInfoB, BLACK);

        Present(true);
    }
}

void Display::ShowUnknownCardScreen(const String& header, const String& code, const String& btnInfoA, const String& btnInfoB)
{
    const String content = header + code + btnInfoA + btnInfoB;

    if (PrepareHash("ShowUnknownCardScreen: " + content))
    {
        _paint.Clear(WHITE);
        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT / 3, header.c_str(), &Font16, BLACK, TextAlignment::CENTER);
        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT / 2, code.c_str(), &Font16, BLACK, TextAlignment::CENTER);

        RenderButtonInfos(btnInfoA, btnInfoB, BLACK);

        Present(true);
    }
}

void Display::ShowInfoScreen(const String& info, const String& btnInfoA, const String& btnInfoB)
{
    if (PrepareHash("ShowInfoScreen: " + info))
    {
        _paint.Clear(WHITE);
        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT / 3, info.c_str(), &Font16, BLACK, TextAlignment::CENTER, 50);

        RenderButtonInfos(btnInfoA, btnInfoB, BLACK);

        Present(true);
    }
}

void Display::ShowErrorScreen(const String& error, const String& btnInfoA, const String& btnInfoB)
{
    Log::Error(error);
    
    if (PrepareHash("ShowErrorScreen: " + error))
    {
        _paint.Clear(WHITE);
        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT / 6, "ERROR", &Font16, BLACK, TextAlignment::CENTER);
        _paint.DrawUtf8StringAt(EPD_WIDTH / 2, EPD_HEIGHT / 3, error.c_str(), &Font16, BLACK, TextAlignment::CENTER, 50);
        RenderButtonInfos(btnInfoA, btnInfoB, BLACK);
        
        Present(true);
    }
}

void Display::ShowFontTestScreen()
{
    sFONT* fnt = &Overpass_mono28;
    auto drawText = [&](const int yOffset, const int height, const int backColor, const int frontColor)
    {
        _paint.DrawFilledRectangle(0, yOffset, EPD_WIDTH, yOffset + height, backColor);

        char c = '0';
        for (int y = yOffset + 5; y < yOffset + height - fnt->Height - 5; y += fnt->Height)
        {
            for (int x = 5; x < EPD_WIDTH - fnt->Width - 5; x += fnt->Width)
            {
                _paint.DrawCharAt(x, y, c, fnt, frontColor);
                c++;
            }
        }
    };

    drawText(0, EPD_HEIGHT / 2, WHITE, BLACK);
    drawText(EPD_HEIGHT / 2, EPD_HEIGHT / 2, RED, WHITE);
    
    Present(false);
}

void Display::RenderFirmwareVersion()
{
    _paint.DrawUtf8StringAt(EPD_WIDTH - 10, EPD_HEIGHT - 20, SMART_SIGN_FW_VERSION, &Font16, BLACK, TextAlignment::RIGHT);
}

void Display::RenderQRCode(const String& message, const int x, const int y, const int size, const int version)
{
    QRCode qrcode;
    std::vector<uint8_t> qrcodeData(qrcode_getBufferSize(version));
    qrcode_initText(&qrcode, qrcodeData.data(), version, ECC_LOW, message.c_str());

    const int boxSize = size / qrcode.size;
    for (uint8_t qy = 0; qy < qrcode.size; qy++)
    {
        for (uint8_t qx = 0; qx < qrcode.size; qx++)
        {
            const int color = qrcode_getModule(&qrcode, qx, qy) ? BLACK : WHITE;
            const int xPos = x + qx * boxSize;
            const int yPos = y + qy * boxSize;
            _paint.DrawFilledRectangle(xPos, yPos, xPos + boxSize - 1, yPos + boxSize - 1, color);
        }
    }
}

void Display::RenderCwaEventQRCode(const ScheduleItem& item)
{
    const int size = 61 * 3;
    const int xCenter = 214 / 2;
    const int yTop = 69 + (214 - size) / 2 + 2;
    const String data = CryptoUtils::CreateCwaEventCode(item, _ctx.Settings().DisplayName);

    RenderQRCode(data.c_str(), xCenter - size / 2, yTop, size, 11);
    _paint.DrawImage(xCenter - IMG_cwa.Width / 2, yTop + size + 25, &IMG_cwa, BLACK, RED);
}

void Display::RenderButtonInfos(const String& btnInfoA, const String& btnInfoB, const int color)
{
    auto renderBtnInfo = [&](const int y, const String& info)
    {
        if (!info.isEmpty())
        {
            _paint.DrawUtf8StringAt(20, y + 14, info.c_str(), &Font16, color);
            _paint.DrawFilledRectangle(0, y, 8, y + 40, color);
        }
    };

    renderBtnInfo(55, btnInfoA);
    renderBtnInfo(EPD_HEIGHT - 86, btnInfoB);
}

bool Display::PrepareHash(const String& content)
{
    // make sure that old hash is null terminated
    rtc_lastHash[MAX_HASH_SIZE - 1] = '\0';
    const String oldHash(rtc_lastHash);

    const String newHash = CryptoUtils::Sha256(content).substring(0, MAX_HASH_SIZE);

    if (newHash != oldHash)
    {
        Log::Debug(content);
        // copy new hash to RTC memory
        newHash.toCharArray(rtc_lastHash, MAX_HASH_SIZE);
        return true;
    }
    return false;
}

void Display::Present(const bool fastMode)
{
    if (!_active || (fastMode && _epd.IsBusy()))
    {
        _active = true;
        _epd.Init();
    }
    Log::Info("e-paper refresh");
    _epd.DisplayFrame(
        _paint, 
        fastMode ? fastInvertedColorPalette : defaultColorPalette,
         false);

    if (fastMode)
    {
        delay(3000);
        _epd.Init();
    }
}

bool Display::IsBusy()
{
    return _active && _epd.IsBusy();
}
