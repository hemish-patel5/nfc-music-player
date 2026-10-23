#include "RotaryEncoder.h"


RotaryEncoder::RotaryEncoder(
    int clkPin,
    int dtPin,
    int buttonPin
)
    : _clkPin(clkPin),
      _dtPin(dtPin),
      _buttonPin(buttonPin),
      _lastCount(0),
      _pendingSteps(0),
      _lastButtonState(HIGH),
      _lastButtonTime(0)
{
}


void RotaryEncoder::begin()
{
    ESP32Encoder::useInternalWeakPullResistors =
        puType::up;


    // Reversed pin order fixes your direction.
    _encoder.attachHalfQuad(
        _dtPin,
        _clkPin
    );


    _encoder.setFilter(1023);

    _encoder.clearCount();

    _lastCount = 0;
    _pendingSteps = 0;


    pinMode(
        _buttonPin,
        INPUT_PULLUP
    );


    _lastButtonState =
        digitalRead(_buttonPin);

    _lastButtonTime =
        millis();
}


EncoderEvent RotaryEncoder::update()
{
    // =====================================
    // ROTATION
    // =====================================

    int64_t rawCount =
        _encoder.getCount();


    /*
        Your encoder is producing roughly
        2 counts per physical click/detent.

        Divide by 2 so one physical click
        becomes one volume step.
    */
    int64_t currentCount =
        rawCount / 2;


    int64_t difference =
        currentCount - _lastCount;


    _lastCount =
        currentCount;


    _pendingSteps +=
        difference;


    if (_pendingSteps > 0)
    {
        _pendingSteps--;

        return EncoderEvent::CLOCKWISE;
    }


    if (_pendingSteps < 0)
    {
        _pendingSteps++;

        return EncoderEvent::COUNTER_CLOCKWISE;
    }


    // =====================================
    // BUTTON
    // =====================================

    int currentButtonState =
        digitalRead(_buttonPin);


    if (
        currentButtonState !=
        _lastButtonState
    )
    {
        unsigned long now =
            millis();


        if (
            now - _lastButtonTime
            >= 50
        )
        {
            _lastButtonTime =
                now;

            _lastButtonState =
                currentButtonState;


            if (
                currentButtonState == LOW
            )
            {
                return
                    EncoderEvent::
                    BUTTON_PRESSED;
            }
        }
    }


    return EncoderEvent::NONE;
}