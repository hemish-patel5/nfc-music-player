#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(
    int clkPin,
    int dtPin,
    int buttonPin
)
    : _clkPin(clkPin),
      _dtPin(dtPin),
      _buttonPin(buttonPin),
      _lastClkState(HIGH),
      _lastButtonState(HIGH),
      _lastButtonTime(0)
{
}

void RotaryEncoder::begin()
{
    pinMode(_clkPin, INPUT_PULLUP);
    pinMode(_dtPin, INPUT_PULLUP);
    pinMode(_buttonPin, INPUT_PULLUP);

    _lastClkState = digitalRead(_clkPin);
    _lastButtonState = digitalRead(_buttonPin);
}

EncoderEvent RotaryEncoder::update()
{
    // -------------------------
    // Rotary movement
    // -------------------------

    int currentClkState = digitalRead(_clkPin);

    if (currentClkState != _lastClkState) {

        // Only detect one edge to avoid counting twice
        if (currentClkState == LOW) {

            int dtState = digitalRead(_dtPin);

            _lastClkState = currentClkState;

            if (dtState != currentClkState) {
                return EncoderEvent::CLOCKWISE;
            } else {
                return EncoderEvent::COUNTER_CLOCKWISE;
            }
        }

        _lastClkState = currentClkState;
    }

    // -------------------------
    // Push button
    // -------------------------

    int currentButtonState =
        digitalRead(_buttonPin);

    if (currentButtonState != _lastButtonState) {

        if (millis() - _lastButtonTime > 50) {

            _lastButtonTime = millis();

            if (currentButtonState == LOW) {

                _lastButtonState =
                    currentButtonState;

                return EncoderEvent::BUTTON_PRESSED;
            }
        }

        _lastButtonState =
            currentButtonState;
    }

    return EncoderEvent::NONE;
}