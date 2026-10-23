#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"


// =============================
// SPI / SD pins
// =============================

constexpr int SPI_SCK  = 18;
constexpr int SPI_MISO = 19;
constexpr int SPI_MOSI = 23;

constexpr int SD_CS = 5;


// =============================
// I2S pins
// =============================

constexpr int I2S_BCLK = 26;
constexpr int I2S_LRC  = 25;
constexpr int I2S_DOUT = 27;


// =============================
// Audio objects
// =============================

AudioGeneratorMP3* mp3 = nullptr;
AudioFileSourceSD* file = nullptr;
AudioOutputI2S* output = nullptr;


void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("============================");
    Serial.println("      MP3 AUDIO TEST");
    Serial.println("============================");
    Serial.println();


    // -----------------------------
    // Start SPI / SD
    // -----------------------------

    pinMode(SD_CS, OUTPUT);

    digitalWrite(
        SD_CS,
        HIGH
    );

    SPI.begin(
        SPI_SCK,
        SPI_MISO,
        SPI_MOSI
    );

    Serial.println("[BOOT] Mounting SD...");

    if (!SD.begin(
        SD_CS,
        SPI,
        4000000
    ))
    {
        Serial.println(
            "[FAIL] SD mount failed"
        );

        return;
    }

    Serial.println(
        "[ OK ] SD mounted"
    );


    // -----------------------------
    // Confirm MP3
    // -----------------------------

    if (!SD.exists("/music/Loser.mp3"))
    {
        Serial.println(
            "[FAIL] Loser.mp3 not found"
        );

        return;
    }

    Serial.println(
        "[ OK ] Loser.mp3 found"
    );


    // -----------------------------
    // Audio output
    // -----------------------------

    Serial.println(
        "[BOOT] Initialising I2S..."
    );

    output =
        new AudioOutputI2S();

    output->SetPinout(
        I2S_BCLK,
        I2S_LRC,
        I2S_DOUT
    );


    // Start low so we don't blast speaker
    output->SetGain(0.40);


    // MAX98357A is mono.
    // This mixes stereo tracks to mono.
    output->SetOutputModeMono(true);


    // -----------------------------
    // MP3 source
    // -----------------------------

    file =
        new AudioFileSourceSD(
            "/music/Loser.mp3"
        );

    mp3 =
        new AudioGeneratorMP3();


    Serial.println(
        "[PLAYER] Starting Loser.mp3..."
    );


    if (!mp3->begin(
        file,
        output
    ))
    {
        Serial.println(
            "[FAIL] MP3 start failed"
        );

        return;
    }


    Serial.println(
        "[ OK ] Playback started"
    );
}


void loop()
{
    if (
        mp3 != nullptr &&
        mp3->isRunning()
    )
    {
        if (!mp3->loop())
        {
            mp3->stop();

            Serial.println();
            Serial.println(
                "[PLAYER] Song finished"
            );
        }
    }
}