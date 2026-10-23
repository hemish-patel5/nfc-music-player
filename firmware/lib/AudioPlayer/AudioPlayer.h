#pragma once

#include <Arduino.h>

#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"


class AudioPlayer
{
public:
    AudioPlayer(
        int bclkPin,
        int lrcPin,
        int dataPin
    );

    bool begin();

    bool play(
        const String& filename
    );

    void stop();

    void update();

    bool isPlaying() const;

    void setVolume(float gain);


private:
    int _bclkPin;
    int _lrcPin;
    int _dataPin;

    AudioGeneratorMP3* _mp3;
    AudioFileSourceSD* _file;
    AudioOutputI2S* _output;

    bool _initialised;
};