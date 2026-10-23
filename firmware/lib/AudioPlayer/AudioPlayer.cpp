#include "AudioPlayer.h"

#include <SD.h>


AudioPlayer::AudioPlayer(
    int bclkPin,
    int lrcPin,
    int dataPin
)
    : _bclkPin(bclkPin),
      _lrcPin(lrcPin),
      _dataPin(dataPin),
      _mp3(nullptr),
      _file(nullptr),
      _output(nullptr),
      _initialised(false),
      _paused(false),
      _volume(10)
{
}


// =====================================
// Begin
// =====================================

bool AudioPlayer::begin()
{
    Serial.println(
        "[AUDIO] Initialising I2S..."
    );

    _output = new AudioOutputI2S();

    if (_output == nullptr)
    {
        Serial.println(
            "[AUDIO] Failed to create output"
        );

        return false;
    }


    _output->SetPinout(
        _bclkPin,
        _lrcPin,
        _dataPin
    );


    // MAX98357A mono output
    _output->SetOutputModeMono(true);


    _mp3 = new AudioGeneratorMP3();

    if (_mp3 == nullptr)
    {
        Serial.println(
            "[AUDIO] Failed to create MP3 decoder"
        );

        return false;
    }


    _initialised = true;


    // Apply starting volume
    setVolume(_volume);


    Serial.println(
        "[AUDIO] Ready"
    );

    return true;
}


// =====================================
// Play
// =====================================

bool AudioPlayer::play(
    const String& filename
)
{
    if (!_initialised)
    {
        Serial.println(
            "[AUDIO] Not initialised"
        );

        return false;
    }


    // Stop previous song
    stop();


    if (!SD.exists(filename))
    {
        Serial.print(
            "[AUDIO] File not found: "
        );

        Serial.println(filename);

        return false;
    }


    Serial.print(
        "[AUDIO] Opening: "
    );

    Serial.println(filename);


    _file = new AudioFileSourceSD(
        filename.c_str()
    );


    if (_file == nullptr)
    {
        Serial.println(
            "[AUDIO] Failed to create file source"
        );

        return false;
    }


    if (!_mp3->begin(
        _file,
        _output
    ))
    {
        Serial.println(
            "[AUDIO] MP3 start failed"
        );

        delete _file;

        _file = nullptr;

        return false;
    }


    _paused = false;


    Serial.println(
        "[AUDIO] Playback started"
    );

    return true;
}


// =====================================
// Stop
// =====================================

void AudioPlayer::stop()
{
    if (_mp3 != nullptr)
    {
        if (_mp3->isRunning())
        {
            _mp3->stop();
        }
    }


    if (_file != nullptr)
    {
        delete _file;

        _file = nullptr;
    }


    _paused = false;
}


// =====================================
// Update
// =====================================

void AudioPlayer::update()
{
    if (_mp3 == nullptr)
    {
        return;
    }


    if (_paused)
    {
        // Do not feed more MP3 data while paused
        return;
    }


    if (_mp3->isRunning())
    {
        if (!_mp3->loop())
        {
            _mp3->stop();

            Serial.println(
                "[AUDIO] Song finished"
            );


            if (_file != nullptr)
            {
                delete _file;

                _file = nullptr;
            }
        }
    }
}


// =====================================
// Pause
// =====================================

void AudioPlayer::pause()
{
    if (_mp3 == nullptr)
    {
        return;
    }


    if (!_mp3->isRunning())
    {
        return;
    }


    if (_paused)
    {
        return;
    }


    _paused = true;

    Serial.println(
        "[AUDIO] Paused"
    );
}


// =====================================
// Resume
// =====================================

void AudioPlayer::resume()
{
    if (_mp3 == nullptr)
    {
        return;
    }


    if (!_mp3->isRunning())
    {
        return;
    }


    if (!_paused)
    {
        return;
    }


    _paused = false;

    Serial.println(
        "[AUDIO] Resumed"
    );
}


// =====================================
// Toggle pause
// =====================================

void AudioPlayer::togglePause()
{
    if (!isPlaying())
    {
        Serial.println(
            "[AUDIO] Nothing playing"
        );

        return;
    }


    if (_paused)
    {
        resume();
    }
    else
    {
        pause();
    }
}


// =====================================
// Status
// =====================================

bool AudioPlayer::isPlaying() const
{
    if (_mp3 == nullptr)
    {
        return false;
    }

    return _mp3->isRunning();
}


bool AudioPlayer::isPaused() const
{
    return _paused;
}


// =====================================
// Volume
// =====================================

void AudioPlayer::setVolume(
    int volume
)
{
    if (volume < 0)
    {
        volume = 0;
    }

    if (volume > 20)
    {
        volume = 20;
    }


    _volume = volume;


    if (_output == nullptr)
    {
        return;
    }


    /*
        Encoder range:

        0  -> silent
        10 -> ~25% gain
        20 -> ~50% gain

        We deliberately cap maximum gain
        at 0.50 for the small speaker.
    */

    float gain =
        (_volume / 20.0f) * 0.50f;


    _output->SetGain(gain);
}


int AudioPlayer::getVolume() const
{
    return _volume;
}