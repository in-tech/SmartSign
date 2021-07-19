#pragma once
#include <Arduino.h>
#include "SchedulePage.h"

class CryptoUtils
{
public:
    static String Sha256(const String& input);
    static String RandomString(const String& alphabet, const int length);
    static String CreateCwaEventCode(const ScheduleItem& item, const String& address);
};