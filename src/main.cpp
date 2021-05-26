#include <Arduino.h>
#include <Esp32Foundation.h>
#include "AppContext.h"

AppContext ctx;

void setup()
{
    Log::Init(115200);
    ctx.Init();
}

void loop()
{
    ctx.Update();
    delay(10);
}