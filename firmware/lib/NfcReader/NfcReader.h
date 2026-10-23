#pragma once

#include <Arduino.h>
#include <MFRC522.h>


class NfcReader
{
public:
    NfcReader(
        int csPin,
        int resetPin
    );

    void begin();

    bool readUid(
        String& uid
    );


private:
    int _csPin;
    int _resetPin;

    MFRC522 _reader;
};