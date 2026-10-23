#include <Arduino.h>

#include "Pins.h"
#include "RotaryEncoder.h"

RotaryEncoder encoder(
    Pins::ENCODER_CLK,
    Pins::ENCODER_DT,
    Pins::ENCODER_SW
);

int volume = 10;
bool paused = false;

void setup()
{
    Serial.begin(115200);

    encoder.begin();

    Serial.println();
    Serial.println("======================");
    Serial.println(" NFC MUSIC PLAYER");
    Serial.println("======================");
    Serial.println("Rotary encoder ready");
    Serial.println();
}

void loop()
{
    EncoderEvent event = encoder.update();

    switch (event) {

        case EncoderEvent::CLOCKWISE:

            if (volume < 20) {
                volume++;
            }

            Serial.print("Volume: ");
            Serial.println(volume);

            break;


        case EncoderEvent::COUNTER_CLOCKWISE:

            if (volume > 0) {
                volume--;
            }

            Serial.print("Volume: ");
            Serial.println(volume);

            break;


        case EncoderEvent::BUTTON_PRESSED:

            paused = !paused;

            if (paused) {
                Serial.println("PAUSED");
            } else {
                Serial.println("PLAYING");
            }

            break;


        case EncoderEvent::NONE:
            break;
    }
}