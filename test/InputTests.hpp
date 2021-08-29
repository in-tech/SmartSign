#pragma once
#include <Arduino.h>
#include <unity.h>
#include "AppContextMock.hpp"

namespace input_tests
{
    bool wait_for_button_press(IAppContext& ctx, const bool btn_a, const bool btn_b, const unsigned long timeout_secs)
    {
        unsigned long start = millis();
        while ((millis() - start) < timeout_secs * 1000)
        {
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

    bool wait_for_end_stop(IAppContext& ctx, const bool on, const unsigned long timeout_secs)
    {
        unsigned long start = millis();
        while ((millis() - start) < timeout_secs * 1000)
        {
            auto inputState = ctx.GetInputMgr().State();
            if (inputState.SmartCardPresent == on)
            {
                return true;
            }
            delay(10);
            ctx.Update();            
        }
        return false;
    }

    void test_button_A()
    {
        AppContextMock1 ctx;
        ctx.Init();
        ctx.Update();
        TEST_ASSERT_TRUE(wait_for_button_press(ctx, false, false, 1));
        Serial.println("Please press button A");
        TEST_ASSERT_TRUE(wait_for_button_press(ctx, true, false, 10));
    }

    void test_button_B()
    {
        AppContextMock1 ctx;
        ctx.Init();
        ctx.Update();
        TEST_ASSERT_TRUE(wait_for_button_press(ctx, false, false, 1));
        Serial.println("Please press button B");
        TEST_ASSERT_TRUE(wait_for_button_press(ctx, false, true, 10));
    }

    void test_smart_card_reader_end_stop()
    {
        AppContextMock1 ctx;
        ctx.Init();
        ctx.Update();
        Serial.println("Please remove smart card");
        TEST_ASSERT_TRUE(wait_for_end_stop(ctx, false, 10));
        Serial.println("Please insert smart card");
        TEST_ASSERT_TRUE(wait_for_end_stop(ctx, true, 10));
    }

    void test_smart_card_id()
    {
        AppContextMock1 ctx;
        ctx.Init();
        ctx.Update();
        const String id = ctx.GetInputMgr().State().SmartCardId;
        Serial.println(String("Card-ID: ") + id);
        TEST_ASSERT_TRUE(id.length() > 0);
    }
}