#pragma once

#include <Arduino.h>
#include <MFRC522.h>

class RFIDScanner {
 public:
  RFIDScanner(uint8_t chipSelectPin, uint8_t resetPin)
      : chipSelectPin_(chipSelectPin), reader_(chipSelectPin, resetPin) {}

  // Start the RC522 and check that the ESP32 can communicate with it.
  bool begin() {
    pinMode(chipSelectPin_, OUTPUT);
    digitalWrite(chipSelectPin_, HIGH);

    reader_.PCD_Init();
    delay(4);

    const byte version = reader_.PCD_ReadRegister(MFRC522::VersionReg);
    return version != 0x00 && version != 0xFF;
  }

  // Return true when a sticker is scanned. Its UID is placed in uid.
  bool scan(String &uid) {
    if (!reader_.PICC_IsNewCardPresent() ||
        !reader_.PICC_ReadCardSerial()) {
      return false;
    }

    uid = "";
    for (byte i = 0; i < reader_.uid.size; i++) {
      if (reader_.uid.uidByte[i] < 0x10) {
        uid += "0";
      }
      uid += String(reader_.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    reader_.PICC_HaltA();
    reader_.PCD_StopCrypto1();
    return true;
  }

 private:
  uint8_t chipSelectPin_;
  MFRC522 reader_;
};
