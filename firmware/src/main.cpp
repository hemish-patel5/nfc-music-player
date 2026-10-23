#include <Arduino.h>
#include <SPI.h>

#include "Pins.h"

#include "RotaryEncoder.h"
#include "SdStorage.h"
#include "ConfigService.h"
#include "NfcReader.h"


// =====================================
// Hardware/services
// =====================================

RotaryEncoder encoder(
    Pins::ENCODER_CLK,
    Pins::ENCODER_DT,
    Pins::ENCODER_SW
);


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


// =====================================
// Player state
// =====================================

int volume = 10;

bool paused = false;


// =====================================
// Setup
// =====================================

void setup()
{
    Serial.begin(115200);

    delay(1000);


    Serial.println();
    Serial.println(
        "================================"
    );

    Serial.println(
        "        NFC MUSIC PLAYER"
    );

    Serial.println(
        "================================"
    );


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
    // Encoder
    // ---------------------------------

    Serial.println();
    Serial.println(
        "[BOOT] Rotary encoder"
    );

    encoder.begin();

    Serial.println(
        "[ OK ] Encoder"
    );


    // ---------------------------------
    // SD
    // ---------------------------------

    Serial.println();
    Serial.println(
        "[BOOT] SD card"
    );


    if (!storage.begin())
    {
        Serial.println(
            "[FAIL] SD card"
        );

        return;
    }


    Serial.println(
        "[ OK ] SD card"
    );


    // ---------------------------------
    // Configuration
    // ---------------------------------

    Serial.println();
    Serial.println(
        "[BOOT] Configuration"
    );


    if (!config.load())
    {
        Serial.println(
            "[FAIL] Configuration"
        );

        return;
    }


    Serial.println(
        "[ OK ] Configuration"
    );


    // ---------------------------------
    // NFC
    // ---------------------------------

    Serial.println();
    Serial.println(
        "[BOOT] NFC reader"
    );


    nfc.begin();


    Serial.println();
    Serial.println(
        "================================"
    );

    Serial.println(
        "          SYSTEM READY"
    );

    Serial.println(
        "================================"
    );

    Serial.println();

    Serial.println(
        "Place an NFC disc on the reader..."
    );
}


// =====================================
// Main loop
// =====================================

void loop()
{
    // =================================
    // Rotary encoder
    // =================================

    EncoderEvent event =
        encoder.update();


    switch (event)
    {
        case EncoderEvent::CLOCKWISE:

            if (volume < 20)
            {
                volume++;
            }

            Serial.print(
                "[VOLUME] "
            );

            Serial.println(volume);

            break;


        case EncoderEvent::COUNTER_CLOCKWISE:

            if (volume > 0)
            {
                volume--;
            }

            Serial.print(
                "[VOLUME] "
            );

            Serial.println(volume);

            break;


        case EncoderEvent::BUTTON_PRESSED:

            paused = !paused;

            Serial.println(
                paused
                    ? "[PLAYER] PAUSED"
                    : "[PLAYER] PLAYING"
            );

            break;


        case EncoderEvent::NONE:
            break;
    }


    // =================================
    // NFC
    // =================================

    String uid;


    if (nfc.readUid(uid))
    {
        Serial.println();

        Serial.println(
            "------------------------------"
        );


        Serial.print(
            "[NFC] UID: "
        );

        Serial.println(uid);


        DiscInfo disc;


        if (!config.findByUid(uid, disc))
        {
            Serial.println(
                "[DISC] Unknown NFC tag"
            );

            Serial.println(
                "------------------------------"
            );

            return;
        }


        Serial.print(
            "[DISC] Disc: "
        );

        Serial.println(
            disc.discId
        );


        // Check whether assigned

        if (
            disc.file.length() == 0
        )
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


        // Check actual SD file

        if (
            storage.exists(
                disc.file
            )
        )
        {
            Serial.println(
                "[ OK ] Music file exists"
            );

            Serial.println(
                "[PLAYER] Ready to play!"
            );
        }
        else
        {
            Serial.println(
                "[FAIL] Music file missing"
            );
        }


        Serial.println(
            "------------------------------"
        );

        Serial.println();
    }
}