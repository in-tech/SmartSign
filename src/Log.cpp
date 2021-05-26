#include "Log.h"

void Log::Init(const unsigned long baud)
{
    Serial.begin(baud);
}

void Log::Info(const String& msg)
{
    SerialLog("[I] ", msg);
}

void Log::Warning(const String& msg)
{
    SerialLog("[W] ", msg);
}

void Log::Error(const String& msg)
{
    SerialLog("[E] ", msg);
}

void Log::Debug(const String& msg)
{
#ifdef DEBUG_LOG
    SerialLog("[D] ", msg);
#endif
}

void Log::SerialLog(const String& prefix, const String& msg)
{
    Serial.print(prefix);
    Serial.println(msg);
}