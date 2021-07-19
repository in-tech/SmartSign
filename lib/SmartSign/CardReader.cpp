#include "CardReader.h"

CardReader::CardReader() :
    _mfrc522(RC522_SS_PIN, RC522_RST_PIN)
{
}

CardReader::~CardReader()
{
}

void CardReader::Init()
{
    pinMode(ENDSTOP_PIN, INPUT);

    SPI.begin();
    pinMode(RC522_SS_PIN, OUTPUT);
    digitalWrite(RC522_SS_PIN, HIGH);
}

bool CardReader::CardIsPresent()
{
    return digitalRead(ENDSTOP_PIN) == HIGH;
}

String CardReader::ReadCardId()
{
    String result = "";
    for (int i = 0; i < 5 && result.isEmpty() && CardIsPresent(); i++)
    {
        PowerUp();
        {
            if (_mfrc522.PICC_IsNewCardPresent() && _mfrc522.PICC_ReadCardSerial())
            {
                for (byte i = 0; i < _mfrc522.uid.size; i++)
                {
                    result.concat(String(_mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
                    result.concat(String(_mfrc522.uid.uidByte[i], HEX));
                }
                result.toUpperCase();
            }
        }
        PowerDown();
    }
    return result;
}

void CardReader::PowerUp()
{
    _mfrc522.PCD_Init();
    delay(2);
}

void CardReader::PowerDown()
{
    _mfrc522.PCD_AntennaOff();
    _mfrc522.PCD_SoftPowerDown();
}