#include "AsciiCompressor.h"

uint16_t AsciiCompressor::Compress(const String& asciiStr, uint8_t* dst, const uint16_t dstSize)
{
    memset(dst, 0, dstSize);
    uint16_t compressedSize = (uint16_t)ceil(float(asciiStr.length()) * 7 / 8) + 1;
    if (dstSize < compressedSize)
    {
        return 0;
    }

    uint16_t dstIdx = 0;
    for (uint16_t i = 0; i < asciiStr.length(); i++)
    {
        uint8_t c = asciiStr[i];
        if ((i + 1) % 8 != 0)
        {
            dst[dstIdx++] = c;
        }
        else
        {
            dst[dstIdx - 7] |= 0x80 & (c << 1);
            dst[dstIdx - 6] |= 0x80 & (c << 2);
            dst[dstIdx - 5] |= 0x80 & (c << 3);
            dst[dstIdx - 4] |= 0x80 & (c << 4);
            dst[dstIdx - 3] |= 0x80 & (c << 5);
            dst[dstIdx - 2] |= 0x80 & (c << 6);
            dst[dstIdx - 1] |= 0x80 & (c << 7);
        }
    }

    return compressedSize;
}

String AsciiCompressor::Decompress(const uint8_t* src, const uint16_t srcSize)
{
    String result;
    result.reserve(srcSize * 8 / 7);

    for (uint16_t i = 0; i < srcSize; i++)
    {
        {
            const char c = (0x7F & src[i]);
            if (c == 0) break;
            result += c;
        }
        
        if ((i + 1) % 7 == 0)
        {
            char c = 0;
            c |= 0x40 & (src[i - 6] >> 1);
            c |= 0x20 & (src[i - 5] >> 2);
            c |= 0x10 & (src[i - 4] >> 3);
            c |= 0x08 & (src[i - 3] >> 4);
            c |= 0x04 & (src[i - 2] >> 5);
            c |= 0x02 & (src[i - 1] >> 6);
            c |= 0x01 & (src[i - 0] >> 7);
            if (c == 0) break;
            result += c;
        }        
    }

    return result;
}