#include "AsciiCompressorTests.hpp"
#include "PowerTests.hpp"
#include "InputTests.hpp"
#include "LedTests.hpp"
#include "DisplayTests.hpp"

void setup()
{
    delay(2000);

    UNITY_BEGIN();

    RUN_TEST(ascii_compressor_tests::test_compression_decompression);

    RUN_TEST(power_tests::test_battery_voltage);

    RUN_TEST(input_tests::test_button_A);
    RUN_TEST(input_tests::test_button_B);

    RUN_TEST(led_tests::test_blink_led);

    RUN_TEST(input_tests::test_smart_card_reader_end_stop);
    RUN_TEST(input_tests::test_smart_card_id);

    RUN_TEST(display_tests::test_info_message);
    RUN_TEST(display_tests::test_font_rendering);
    RUN_TEST(display_tests::test_booked_schedule_screen);
    RUN_TEST(display_tests::test_free_schedule_screen);

    UNITY_END();
}

void loop()
{
}