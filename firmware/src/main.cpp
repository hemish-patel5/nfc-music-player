#include <Arduino.h>
#include <SPI.h>

#include "Pins.h"
#include "SdStorage.h"
#include "ConfigService.h"
#include "NfcReader.h"


// =====================================
// Services
// =====================================

SdStorage storage(
    Pins::SD_CS,
    Pins::SPI_SCK,
    Pins::SPI_MISO,
    Pins::SPI_MOSI
);

ConfigService config(storage);

NfcReader nfc(
    Pins::RFID_CS,
    Pins::RFID_RST
);


// =====================================
// Setup
// =====================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("       NFC MUSIC PLAYER");
    Serial.println("      NFC + SD + CONFIG");
    Serial.println("================================");
    Serial.println();


    // ---------------------------------
    // Prepare SPI chip selects
    // ---------------------------------

    pinMode(Pins::SD_CS, OUTPUT);
    pinMode(Pins::RFID_CS, OUTPUT);

    digitalWrite(Pins::SD_CS, HIGH);
    digitalWrite(Pins::RFID_CS, HIGH);

    delay(100);


    // ---------------------------------
    // Start shared SPI bus ONCE
    // ---------------------------------

    SPI.begin(
        Pins::SPI_SCK,
        Pins::SPI_MISO,
        Pins::SPI_MOSI
    );

    delay(100);


    // ---------------------------------
    // SD card
    // ---------------------------------

    Serial.println("[BOOT] SD card");

    if (!storage.begin())
    {
        Serial.println("[FAIL] SD card");
        return;
    }

    Serial.println("[ OK ] SD card");


    // ---------------------------------
    // Config
    // ---------------------------------

    Serial.println();
    Serial.println("[BOOT] Config");

    if (!config.load())
    {
        Serial.println("[FAIL] Config");
        return;
    }

    Serial.println("[ OK ] Config");

    Serial.print("[CONFIG] Version: ");
    Serial.println(config.getVersion());

    Serial.print("[CONFIG] Discs: ");
    Serial.println(config.getDiscCount());


    // ---------------------------------
    // NFC reader
    // ---------------------------------

    Serial.println();
    Serial.println("[BOOT] NFC reader");

    nfc.begin();

    Serial.println("[ OK ] NFC reader");


    Serial.println();
    Serial.println("================================");
    Serial.println("          SYSTEM READY");
    Serial.println("================================");
    Serial.println();

    Serial.println(
        "Place a sticker near the reader..."
    );
}


// =====================================
// Main loop
// =====================================

void loop()
{
    String uid;


    // ---------------------------------
    // Try reading NFC tag
    // ---------------------------------

    if (!nfc.readUid(uid))
    {
        return;
    }


    Serial.println();
    Serial.println("------------------------------");

    Serial.print("[NFC] UID: ");
    Serial.println(uid);


    // ---------------------------------
    // Find disc in config
    // ---------------------------------

    DiscInfo disc;

    if (!config.findByUid(uid, disc))
    {
        Serial.println(
            "[DISC] Unknown NFC sticker"
        );

        Serial.println("------------------------------");

        delay(500);

        return;
    }


    Serial.print("[DISC] Disc #: ");
    Serial.println(disc.discId);


    // ---------------------------------
    // Check whether assigned
    // ---------------------------------

    if (disc.file.length() == 0)
    {
        Serial.println(
            "[DISC] No song assigned"
        );

        Serial.println("------------------------------");

        delay(500);

        return;
    }


    Serial.print("[DISC] Title: ");
    Serial.println(disc.title);

    Serial.print("[DISC] File: ");
    Serial.println(disc.file);


    // ---------------------------------
    // Confirm MP3 file exists
    // ---------------------------------

    if (storage.exists(disc.file))
    {
        Serial.println(
            "[ OK ] MP3 file exists"
        );

        Serial.println(
            "[PLAYER] Ready to play"
        );
    }
    else
    {
        Serial.println(
            "[FAIL] MP3 file missing"
        );
    }


    Serial.println("------------------------------");

    delay(1000);
}