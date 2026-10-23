#pragma once

#include <Arduino.h>

class SdStorage
{
public:
    SdStorage(
        int csPin,
        int sckPin,
        int misoPin,
        int mosiPin
    );

    bool begin();

    bool exists(const String& path);

    bool readFile(
        const String& path,
        String& output
    );

    void printCardInfo();

    void listDirectory(
        const String& path
    );

private:
    int _csPin;
    int _sckPin;
    int _misoPin;
    int _mosiPin;

    bool _mounted;
};