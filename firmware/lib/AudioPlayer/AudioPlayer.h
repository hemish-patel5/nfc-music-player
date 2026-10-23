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


    // Playback control
    void pause();
    void resume();
    void togglePause();

    bool isPlaying() const;
    bool isPaused() const;


    // Volume: 0 - 20
    void setVolume(int volume);

    int getVolume() const;


private:
    int _bclkPin;
    int _lrcPin;
    int _dataPin;

    AudioGeneratorMP3* _mp3;
    AudioFileSourceSD* _file;
    AudioOutputI2S* _output;

    bool _initialised;
    bool _paused;

    int _volume;
};