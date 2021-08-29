#pragma once
#include <Arduino.h>
#include <unity.h>
#include "AppContextMock.hpp"

namespace led_tests
{
    void test_blink_led()
    {
        AppContextMock1 ctx;
        ctx.Init();
        ctx.Update();
        Serial.println("Please press button A if LED is blinking.");

        unsigned long start = millis();
        while ((millis() - start) < 10000)
        {
            auto inputState = ctx.GetInputMgr().State();
            if (inputState.PressedButtonA)
            {
                TEST_ASSERT_TRUE(true);
                return;
            }

            ctx.GetPowerMgr().SetStatusLED((millis() / 500) % 2 == 0);

            delay(100);
            ctx.Update();            
        }
        
        TEST_ASSERT_TRUE(false);
    }
}