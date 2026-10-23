#include <Arduino.h>
#include <SPI.h>

#include "Pins.h"

#include "SdStorage.h"
#include "ConfigService.h"
#include "NfcReader.h"
#include "AudioPlayer.h"
#include "RotaryEncoder.h"


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


RotaryEncoder encoder(
    Pins::ENCODER_CLK,
    Pins::ENCODER_DT,
    Pins::ENCODER_SW
);


// =====================================
// State
// =====================================

String lastUid = "";

int volume = 10;


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
        "       NFC MUSIC PLAYER"
    );

    Serial.println(
        "================================"
    );

    Serial.println();


    // =================================
    // SPI
    // =================================

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


    SPI.begin(
        Pins::SPI_SCK,
        Pins::SPI_MISO,
        Pins::SPI_MOSI
    );


    delay(100);


    // =================================
    // Encoder
    // =================================

    Serial.println(
        "[BOOT] Encoder"
    );

    encoder.begin();

    Serial.println(
        "[ OK ] Encoder"
    );


    // =================================
    // SD
    // =================================

    Serial.println(
        "[BOOT] SD"
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


    // =================================
    // Config
    // =================================

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


    // =================================
    // NFC
    // =================================

    Serial.println(
        "[BOOT] NFC"
    );

    nfc.begin();

    Serial.println(
        "[ OK ] NFC"
    );


    // =================================
    // Audio
    // =================================

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


    // Initial volume
    audio.setVolume(volume);


    Serial.println(
        "[ OK ] Audio"
    );


    Serial.print(
        "[VOLUME] "
    );

    Serial.println(volume);


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
        "Scan an NFC disc..."
    );
}


// =====================================
// Main loop
// =====================================

void loop()
{
    // =================================
    // AUDIO
    // =================================

    // Very important:
    // this must run constantly
    audio.update();


    // =================================
    // ENCODER
    // =================================

    EncoderEvent event =
        encoder.update();


    switch (event)
    {
        // -----------------------------
        // Volume up
        // -----------------------------

        case EncoderEvent::CLOCKWISE:
        {
            if (volume < 20)
            {
                volume++;
            }


            audio.setVolume(volume);


            Serial.print(
                "[VOLUME] "
            );

            Serial.print(volume);

            Serial.println(
                " / 20"
            );

            break;
        }


        // -----------------------------
        // Volume down
        // -----------------------------

        case EncoderEvent::COUNTER_CLOCKWISE:
        {
            if (volume > 0)
            {
                volume--;
            }


            audio.setVolume(volume);


            Serial.print(
                "[VOLUME] "
            );

            Serial.print(volume);

            Serial.println(
                " / 20"
            );

            break;
        }


        // -----------------------------
        // Play / Pause
        // -----------------------------

        case EncoderEvent::BUTTON_PRESSED:
        {
            audio.togglePause();


            if (audio.isPlaying())
            {
                if (audio.isPaused())
                {
                    Serial.println(
                        "[PLAYER] PAUSED"
                    );
                }
                else
                {
                    Serial.println(
                        "[PLAYER] PLAYING"
                    );
                }
            }


            break;
        }


        case EncoderEvent::NONE:
            break;
    }


    // =================================
    // NFC
    // =================================

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


    // =================================
    // Same disc protection
    // =================================

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


    // =================================
    // Config lookup
    // =================================

    DiscInfo disc;


    if (!config.findByUid(
        uid,
        disc
    ))
    {
        Serial.println(
            "[DISC] Unknown NFC disc"
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


    // =================================
    // Check MP3
    // =================================

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


    // =================================
    // Play song
    // =================================

    Serial.println(
        "[PLAYER] Starting..."
    );


    if (audio.play(
        disc.file
    ))
    {
        lastUid = uid;

        Serial.println(
            "[PLAYER] PLAYING"
        );
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