#pragma once
#include <Arduino.h>
#include "Log.h"
#include "AsciiCompressor.h"

template<int SIZE>
struct AzureToken
{
    bool Valid;
    uint8_t CompressedToken[SIZE];
    
    String Get()
    {
        if (!Valid)
        {
            return "";
        }
        return AsciiCompressor::Decompress(CompressedToken, SIZE);
    }

    bool Set(const String& token)
    {
        if (AsciiCompressor::Compress(token, CompressedToken, SIZE) == 0)
        {
            Valid = false;
            Log::Error(F("buffer too small for compressed token"));
            return false;
        }
        Valid = true;
        return true;
    }
};
