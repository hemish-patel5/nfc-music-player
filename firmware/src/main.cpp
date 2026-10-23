#include <Arduino.h>

#include "Pins.h"
#include "SdStorage.h"


SdStorage storage(
    Pins::SD_CS,
    Pins::SPI_SCK,
    Pins::SPI_MISO,
    Pins::SPI_MOSI
);


void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("============================");
    Serial.println("      NFC MUSIC PLAYER");
    Serial.println("        SD CARD TEST");
    Serial.println("============================");
    Serial.println();


    if (!storage.begin())
    {
        Serial.println();
        Serial.println("[FAIL] SD card test failed");

        return;
    }


    Serial.println();
    storage.printCardInfo();


    // List root directory

    Serial.println();
    storage.listDirectory("/");


    // List music directory

    Serial.println();
    storage.listDirectory("/music");


    // Check TESLA

    Serial.println();

    if (storage.exists("/music/TESLA.mp3"))
    {
        Serial.println(
            "[ OK ] TESLA.mp3 found"
        );
    }
    else
    {
        Serial.println(
            "[FAIL] TESLA.mp3 not found"
        );
    }


    // Check Loser

    if (storage.exists("/music/Loser.mp3"))
    {
        Serial.println(
            "[ OK ] Loser.mp3 found"
        );
    }
    else
    {
        Serial.println(
            "[FAIL] Loser.mp3 not found"
        );
    }


    // Check config

    if (
        storage.exists(
            "/config/config.json"
        )
    )
    {
        Serial.println(
            "[ OK ] config.json found"
        );
    }
    else
    {
        Serial.println(
            "[FAIL] config.json not found"
        );
    }


    // Read config.json

    String config;

    if (
        storage.readFile(
            "/config/config.json",
            config
        )
    )
    {
        Serial.println();
        Serial.println(
            "========== CONFIG =========="
        );

        Serial.println(config);

        Serial.println(
            "============================"
        );
    }


    Serial.println();
    Serial.println(
        "[SUCCESS] SD card test complete"
    );
}


void loop()
{
}