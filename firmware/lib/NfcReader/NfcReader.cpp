#include "NfcReader.h"

NfcReader::NfcReader(
    int csPin,
    int resetPin
)
    : _csPin(csPin),
      _resetPin(resetPin),
      _reader(csPin, resetPin)
{
}

void NfcReader::begin()
{
    Serial.println("[NFC] Initialising RC522...");

    _reader.PCD_Init();

    delay(100);

    Serial.println("[NFC] RC522 ready");
}

bool NfcReader::readUid(String& uid)
{
    if (!_reader.PICC_IsNewCardPresent())
    {
        return false;
    }

    if (!_reader.PICC_ReadCardSerial())
    {
        return false;
    }

    uid = "";

    for (byte i = 0; i < _reader.uid.size; i++)
    {
        if (_reader.uid.uidByte[i] < 0x10)
        {
            uid += "0";
        }

        uid += String(
            _reader.uid.uidByte[i],
            HEX
        );
    }

    uid.toUpperCase();

    _reader.PICC_HaltA();
    _reader.PCD_StopCrypto1();

    return true;
}