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
}