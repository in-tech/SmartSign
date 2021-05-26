#pragma once
#include <Arduino.h>
#include <MFRC522.h>
#include "Config.h"

class CardReader
{
public:
    CardReader();
    ~CardReader();

    void Init();

    bool CardIsPresent();
    String ReadCardId();

private:
    void PowerUp();
    void PowerDown();

private:
    MFRC522 _mfrc522;
};