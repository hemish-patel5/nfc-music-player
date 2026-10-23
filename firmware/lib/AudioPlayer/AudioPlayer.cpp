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
      _initialised(false)
{
}


bool AudioPlayer::begin()
{
    Serial.println(
        "[AUDIO] Initialising I2S..."
    );

    _output = new AudioOutputI2S();

    if (_output == nullptr)
    {
        Serial.println(
            "[AUDIO] Failed to create I2S output"
        );

        return false;
    }


    _output->SetPinout(
        _bclkPin,
        _lrcPin,
        _dataPin
    );


    // Start at a safe volume
    _output->SetGain(0.30);

    // Your MAX98357A is mono
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


    Serial.println(
        "[AUDIO] Ready"
    );


    return true;
}


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


    // Stop current song first
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


    _file =
        new AudioFileSourceSD(
            filename.c_str()
        );


    if (_file == nullptr)
    {
        Serial.println(
            "[AUDIO] Failed to open file"
        );

        return false;
    }


    if (!_mp3->begin(
        _file,
        _output
    ))
    {
        Serial.println(
            "[AUDIO] MP3 begin failed"
        );

        delete _file;

        _file = nullptr;

        return false;
    }


    Serial.println(
        "[AUDIO] Playback started"
    );


    return true;
}


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
}


void AudioPlayer::update()
{
    if (_mp3 == nullptr)
    {
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


bool AudioPlayer::isPlaying() const
{
    if (_mp3 == nullptr)
    {
        return false;
    }

    return _mp3->isRunning();
}


void AudioPlayer::setVolume(
    float gain
)
{
    if (_output == nullptr)
    {
        return;
    }


    if (gain < 0.0)
    {
        gain = 0.0;
    }

    if (gain > 1.0)
    {
        gain = 1.0;
    }


    _output->SetGain(gain);
}