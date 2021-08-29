#pragma once
#include <Arduino.h>
#include <unity.h>
#include "AppContextMock.hpp"

namespace power_tests
{
    void test_battery_voltage()
    {
        AppContextMock1 ctx;
        ctx.Init();
        ctx.Update();
        const float voltage = ctx.GetPowerMgr().GetBatteryVoltage();
        Serial.println(String("Battery voltage: ") + voltage);
        TEST_ASSERT_TRUE(voltage > 2.5f);
        TEST_ASSERT_TRUE(voltage < 4.4f);
    }
}