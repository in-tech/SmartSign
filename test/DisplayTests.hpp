#pragma once
#include <Arduino.h>
#include <unity.h>
#include "Display.h"
#include "AppContextMock.hpp"

namespace display_tests
{
    class AppContextDisplayMock : public AppContextMock1
    {
    public:
        AppContextDisplayMock()
            : _display(*this)
        {
        }

        IDisplay &GetDisplay() { return _display; };

        void Init()
        {
            AppContextMock1::Init();
            _display.Init();
        };

        void DeInit()
        {
            AppContextMock1::DeInit();
            _display.DeInit();
        }

    private:
        Display _display;
    };

    bool wait_for_button_press(IAppContext &ctx, const bool btn_a, const bool btn_b, const unsigned long timeout_secs)
    {
        unsigned long start = millis();
        while ((millis() - start) < timeout_secs * 1000)
        {
            if (ctx.GetDisplay().IsBusy())
            {
                start = millis();
            }

            auto inputState = ctx.GetInputMgr().State();
            if (inputState.PressedButtonA == btn_a && inputState.PressedButtonB == btn_b)
            {
                return true;
            }
            delay(10);
            ctx.Update();
        }
        return false;
    }

    void test_info_message()
    {
        AppContextDisplayMock ctx;
        ctx.Init();
        ctx.GetDisplay().ShowInfoScreen("display_tests::test_info_message", "Button A", "Button B");
        ctx.Update();

        Serial.println("Please press button A if you can read the message 'display_tests::test_info_message' on the display.");
        bool success = wait_for_button_press(ctx, true, false, 10);
        TEST_ASSERT_TRUE(success);
    }

    void test_font_rendering()
    {
        AppContextDisplayMock ctx;
        ctx.Init();
        ctx.GetDisplay().ShowFontTestScreen();
        ctx.Update();

        Serial.println("Please press button A if font rendering looks all right on the display.");
        bool success = wait_for_button_press(ctx, true, false, 10);
        TEST_ASSERT_TRUE(success);
    }

    void test_booked_schedule_screen()
    {
        AppContextDisplayMock ctx;
        ctx.Init();

        time_t utcNow = TimeUtils::UtcIsoStringToTime("2021-04-01T09:52:00");
        time_t localNow = utcNow;
        std::vector<ScheduleItem> items;
        {
            ScheduleItem item = {};
            item.Subject = "Heinz, Karl";
            item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T06:30:00");
            item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T06:35:00");
            items.push_back(item);
        }

        {
            ScheduleItem item = {};
            item.Subject = "Wurst, Hans";
            item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T07:30:00");
            item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T10:00:00");
            items.push_back(item);
        }

        {
            ScheduleItem item = {};
            item.Subject = "Mustermann, Max 0123456789ABCDEF";
            item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T10:00:00");
            item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T14:30:00");
            items.push_back(item);
        }

        {
            ScheduleItem item = {};
            item.Subject = "Roßbach, Marc";
            item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T15:00:00");
            item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T16:00:00");
            items.push_back(item);
        }

        ctx.GetDisplay().ShowScheduleScreen(localNow, items);

        Serial.println("Please press button A if BOOKED schedule looks all right on the display.");
        bool success = wait_for_button_press(ctx, true, false, 10);
        TEST_ASSERT_TRUE(success);
    }

    void test_free_schedule_screen()
    {
        AppContextDisplayMock ctx;
        ctx.Init();

        time_t utcNow = TimeUtils::UtcIsoStringToTime("2021-04-01T09:52:00");
        time_t localNow = utcNow;
        std::vector<ScheduleItem> items;

        {
            ScheduleItem item = {};
            item.Subject = "Mustermann, Max 0123456789ABCDEF";
            item.LocalStartTime = TimeUtils::UtcIsoStringToTime("2021-04-01T10:00:00");
            item.LocalEndTime = TimeUtils::UtcIsoStringToTime("2021-04-01T14:30:00");
            items.push_back(item);
        }

        ctx.GetDisplay().ShowScheduleScreen(localNow, items);

        Serial.println("Please press button A if FREE schedule looks all right on the display.");
        bool success = wait_for_button_press(ctx, true, false, 10);
        TEST_ASSERT_TRUE(success);
    }
}