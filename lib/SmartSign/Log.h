#pragma once
#include <Arduino.h>

class Log
{
public:
    static void Init(const unsigned long baud);

    static void Info(const String& msg);
    static void Warning(const String& msg);
    static void Error(const String& msg);
    static void Debug(const String& msg);

private:
    static void SerialLog(const String& prefix, const String& msg);
};