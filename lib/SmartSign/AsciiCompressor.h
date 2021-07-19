#pragma once
#include <Arduino.h>

class AsciiCompressor
{
public:
    static uint16_t GetCompressedSize(const String& asciiStr);
    static uint16_t Compress(const String& asciiStr, uint8_t* dst, const uint16_t dstSize);
    static String Decompress(const uint8_t* src, const uint16_t srcSize);
};