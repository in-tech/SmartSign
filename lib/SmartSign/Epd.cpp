/**
 *  @filename   :   epd7in5b.cpp
 *  @brief      :   Implements for e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "Epd.h"

Epd::~Epd()
{
}

Epd::Epd()
{
    reset_pin = EPD_RST_PIN;
    dc_pin = EPD_DC_PIN;
    cs_pin = EPD_CS_PIN;
    busy_pin = EPD_BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
}

int Epd::Init(void)
{
    if (IfInit() != 0)
    {
        return -1;
    }
    Reset();

    SendCommand(POWER_SETTING);
    SendDataTransacted(0x37);
    SendDataTransacted(0x00);

    SendCommand(PANEL_SETTING);
    SendDataTransacted(0xCF);
    SendDataTransacted(0x08);

    SendCommand(BOOSTER_SOFT_START);
    SendDataTransacted(0xc7);
    SendDataTransacted(0xcc);
    SendDataTransacted(0x28);

    SendCommand(POWER_ON);
    WaitUntilIdle();

    SendCommand(PLL_CONTROL);
    SendDataTransacted(0x3c);

    SendCommand(TEMPERATURE_CALIBRATION);
    SendDataTransacted(0x00);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendDataTransacted(0x77);

    SendCommand(TCON_SETTING);
    SendDataTransacted(0x22);

    SendCommand(TCON_RESOLUTION);
    SendDataTransacted(0x02); //source 640
    SendDataTransacted(0x80);
    SendDataTransacted(0x01); //gate 384
    SendDataTransacted(0x80);

    SendCommand(VCM_DC_SETTING);
    SendDataTransacted(0x1E); //decide by LUT file

    SendCommand(0xe5); //FLASH MODE
    SendDataTransacted(0x03);

    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command)
{
    DigitalWrite(dc_pin, LOW);
    SpiTransferTransacted(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendDataTransacted(unsigned char data)
{
    DigitalWrite(dc_pin, HIGH);
    SpiTransferTransacted(data);
}

void Epd::SendData(unsigned char data)
{
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

bool Epd::IsBusy(void)
{
    return DigitalRead(busy_pin) == 0; // 0: busy, 1: idle
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void)
{
    while (IsBusy())
    {
        DelayMs(10);
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void)
{
    DigitalWrite(reset_pin, LOW); //module reset
    DelayMs(100);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(10);
}

void Epd::DisplayFrame(Paint &img, const unsigned char* palette, const bool wait)
{
    SendCommand(DATA_START_TRANSMISSION_1);

    if (img.GetBitsPerPixel() == TWO_BITS)
    {
        SpiBeginTransaction();
        const int numBytes = img.GetWidth() * img.GetHeight() / 4;
        for (int i = 0; i < numBytes; ++i)
        {
            SendFourPixels(img.GetImage()[i], palette);
        }
        SpiEndTransaction();
    }
    else
    {
        // not implemented
        Serial.println("this pixel format is not implemented");
    }

    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    if (wait)
    {
        WaitUntilIdle();
    }
}

void Epd::DisplayColorFrame(const unsigned char color, const bool wait)
{
    SendCommand(DATA_START_TRANSMISSION_1);

    SpiBeginTransaction();
    const unsigned char pixels[2] = { color, color };
    for (int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 2; i++)
    {
        SendTwoPixels(pixels, defaultColorPalette);
    }
    SpiEndTransaction();

    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    if (wait)
    {
        WaitUntilIdle();
    }
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5. 
 *          You can use EPD_Reset() to awaken
 */
void Epd::Sleep(void)
{
    SendCommand(POWER_OFF);
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);
    SendDataTransacted(0xa5);
}

void Epd::SendTwoPixels(const unsigned char *pixels, const unsigned char* palette)
{
    unsigned char value = 0; // black
    for (long i = 0; i < 2; i++)
    {
        value |= palette[pixels[i]];
        if (i == 0)
        {
            value <<= 4;
        }
    }
    SendData(value);
}

void Epd::SendFourPixels(const unsigned char pixels, const unsigned char* palette)
{
    unsigned char unpackedPixels[4];
    for (long i = 0; i < 4; i++)
    {
        unpackedPixels[3 - i] = (pixels >> (i * 2)) & 0x3;
    }
    SendTwoPixels(&unpackedPixels[0], palette);
    SendTwoPixels(&unpackedPixels[2], palette);
}