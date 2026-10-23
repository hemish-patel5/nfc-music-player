#pragma once

#include <Arduino.h>

enum class EncoderEvent {
    NONE,
    CLOCKWISE,
    COUNTER_CLOCKWISE,
    BUTTON_PRESSED
};

class RotaryEncoder {

public:
    RotaryEncoder(
        int clkPin,
        int dtPin,
        int buttonPin
    );

    void begin();

    EncoderEvent update();

private:
    int _clkPin;
    int _dtPin;
    int _buttonPin;

    int _lastClkState;
    int _lastButtonState;

    unsigned long _lastButtonTime;
};