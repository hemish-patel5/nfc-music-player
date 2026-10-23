#include <Arduino.h>
#include <SPI.h>

#include "Pins.h"
#include "SdStorage.h"
#include "ConfigService.h"
#include "NfcReader.h"
#include "AudioPlayer.h"


// =====================================
// Services
// =====================================

SdStorage storage(
    Pins::SD_CS,
    Pins::SPI_SCK,
    Pins::SPI_MISO,
    Pins::SPI_MOSI
);


ConfigService config(
    storage
);


NfcReader nfc(
    Pins::RFID_CS,
    Pins::RFID_RST
);


AudioPlayer audio(
    Pins::I2S_BCLK,
    Pins::I2S_LRC,
    Pins::I2S_DOUT
);


// =====================================
// State
// =====================================

String lastUid = "";


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
    Serial.println("     NFC + SD + AUDIO TEST");
    Serial.println("================================");
    Serial.println();


    // ---------------------------------
    // Prepare SPI devices
    // ---------------------------------

    pinMode(
        Pins::SD_CS,
        OUTPUT
    );

    pinMode(
        Pins::RFID_CS,
        OUTPUT
    );


    digitalWrite(
        Pins::SD_CS,
        HIGH
    );

    digitalWrite(
        Pins::RFID_CS,
        HIGH
    );


    // ---------------------------------
    // Shared SPI bus
    // ---------------------------------

    SPI.begin(
        Pins::SPI_SCK,
        Pins::SPI_MISO,
        Pins::SPI_MOSI
    );


    delay(100);


    // ---------------------------------
    // SD
    // ---------------------------------

    Serial.println(
        "[BOOT] SD card"
    );


    if (!storage.begin())
    {
        Serial.println(
            "[FAIL] SD"
        );

        return;
    }


    Serial.println(
        "[ OK ] SD"
    );


    // ---------------------------------
    // Config
    // ---------------------------------

    Serial.println();
    Serial.println(
        "[BOOT] Config"
    );


    if (!config.load())
    {
        Serial.println(
            "[FAIL] Config"
        );

        return;
    }


    Serial.println(
        "[ OK ] Config"
    );


    // ---------------------------------
    // NFC
    // ---------------------------------

    Serial.println();
    Serial.println(
        "[BOOT] NFC"
    );


    nfc.begin();


    Serial.println(
        "[ OK ] NFC"
    );


    // ---------------------------------
    // Audio
    // ---------------------------------

    Serial.println();
    Serial.println(
        "[BOOT] Audio"
    );


    if (!audio.begin())
    {
        Serial.println(
            "[FAIL] Audio"
        );

        return;
    }


    Serial.println(
        "[ OK ] Audio"
    );


    Serial.println();
    Serial.println("================================");
    Serial.println("          SYSTEM READY");
    Serial.println("================================");
    Serial.println();

    Serial.println(
        "Scan sticker #1 or #2..."
    );
}


// =====================================
// Main loop
// =====================================

void loop()
{
    // IMPORTANT:
    // Audio must be serviced continuously.
    audio.update();


    // ---------------------------------
    // NFC
    // ---------------------------------

    String uid;


    if (!nfc.readUid(uid))
    {
        return;
    }


    Serial.println();
    Serial.println(
        "------------------------------"
    );


    Serial.print(
        "[NFC] UID: "
    );

    Serial.println(uid);


    // ---------------------------------
    // Prevent immediate restart
    // ---------------------------------

    if (uid == lastUid)
    {
        Serial.println(
            "[NFC] Same disc already selected"
        );

        Serial.println(
            "------------------------------"
        );

        return;
    }


    // ---------------------------------
    // Config lookup
    // ---------------------------------

    DiscInfo disc;


    if (!config.findByUid(
        uid,
        disc
    ))
    {
        Serial.println(
            "[DISC] Unknown sticker"
        );

        Serial.println(
            "------------------------------"
        );

        return;
    }


    Serial.print(
        "[DISC] Disc #: "
    );

    Serial.println(
        disc.discId
    );


    if (disc.file.length() == 0)
    {
        Serial.println(
            "[DISC] No song assigned"
        );

        Serial.println(
            "------------------------------"
        );

        return;
    }


    Serial.print(
        "[DISC] Title: "
    );

    Serial.println(
        disc.title
    );


    Serial.print(
        "[DISC] File: "
    );

    Serial.println(
        disc.file
    );


    // ---------------------------------
    // Check file
    // ---------------------------------

    if (!storage.exists(
        disc.file
    ))
    {
        Serial.println(
            "[FAIL] MP3 missing"
        );

        Serial.println(
            "------------------------------"
        );

        return;
    }


    // ---------------------------------
    // Play
    // ---------------------------------

    Serial.println(
        "[PLAYER] Starting song..."
    );


    if (
        audio.play(
            disc.file
        )
    )
    {
        Serial.println(
            "[PLAYER] Playing"
        );

        lastUid = uid;
    }
    else
    {
        Serial.println(
            "[PLAYER] Failed"
        );
    }


    Serial.println(
        "------------------------------"
    );
}