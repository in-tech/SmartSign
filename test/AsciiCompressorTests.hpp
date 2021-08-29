#pragma once
#include <Arduino.h>
#include <unity.h>
#include "AsciiCompressor.h"

namespace ascii_compressor_tests
{
    void test_compression_decompression()
    {
        const String input("Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.");
        uint8_t buffer[256];
        const uint16_t compressed_size = AsciiCompressor::Compress(input, buffer, 256);
        TEST_ASSERT_GREATER_THAN(0, compressed_size);
        TEST_ASSERT_LESS_THAN(input.length(), compressed_size);

        const String output = AsciiCompressor::Decompress(buffer, compressed_size);
        TEST_ASSERT_TRUE(input == output);
    }
}