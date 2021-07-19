#pragma once
#include <Arduino.h>
#include "Log.h"
#include "AsciiCompressor.h"

template<uint16_t SIZE>
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
            const auto compressed_size = AsciiCompressor::GetCompressedSize(token);
            Log::Error(String("rtc-buffer too small for compressed token (") + compressed_size + " vs " + SIZE + ")");
            return false;
        }
        Valid = true;
        return true;
    }
};
