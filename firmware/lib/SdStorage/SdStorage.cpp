#include "SdStorage.h"

#include <SPI.h>
#include <SD.h>


SdStorage::SdStorage(
    int csPin,
    int sckPin,
    int misoPin,
    int mosiPin
)
    : _csPin(csPin),
      _sckPin(sckPin),
      _misoPin(misoPin),
      _mosiPin(mosiPin),
      _mounted(false)
{
}


bool SdStorage::begin()
{
    Serial.println("[SD] Initialising...");

    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);

    if (!SD.begin(
        _csPin,
        SPI,
        4000000
    ))
    {
        Serial.println("[SD] ERROR: Mount failed");

        _mounted = false;

        return false;
    }

    if (SD.cardType() == CARD_NONE)
    {
        Serial.println(
            "[SD] ERROR: No SD card detected"
        );

        _mounted = false;

        return false;
    }

    _mounted = true;

    Serial.println(
        "[SD] Card mounted successfully"
    );

    return true;
}


bool SdStorage::exists(const String& path)
{
    if (!_mounted)
    {
        return false;
    }

    return SD.exists(path);
}


bool SdStorage::readFile(
    const String& path,
    String& output
)
{
    if (!_mounted)
    {
        Serial.println("[SD] Card not mounted");

        return false;
    }

    File file = SD.open(path);

    if (!file)
    {
        Serial.print("[SD] Failed to open: ");
        Serial.println(path);

        return false;
    }

    if (file.isDirectory())
    {
        Serial.print("[SD] Path is a directory: ");
        Serial.println(path);

        file.close();

        return false;
    }

    output = "";

    while (file.available())
    {
        output += static_cast<char>(file.read());
    }

    file.close();

    return true;
}


void SdStorage::printCardInfo()
{
    if (!_mounted)
    {
        return;
    }

    uint8_t cardType = SD.cardType();

    Serial.print("[SD] Card type: ");

    switch (cardType)
    {
        case CARD_MMC:
            Serial.println("MMC");
            break;

        case CARD_SD:
            Serial.println("SDSC");
            break;

        case CARD_SDHC:
            Serial.println("SDHC");
            break;

        default:
            Serial.println("UNKNOWN");
            break;
    }

    uint64_t cardSize =
        SD.cardSize() / (1024ULL * 1024ULL);

    Serial.print("[SD] Size: ");
    Serial.print((unsigned long)cardSize);
    Serial.println(" MB");
}


void SdStorage::listDirectory(
    const String& path
)
{
    if (!_mounted)
    {
        return;
    }

    Serial.print("[SD] Listing: ");
    Serial.println(path);

    File root = SD.open(path);

    if (!root)
    {
        Serial.println(
            "[SD] Failed to open directory"
        );

        return;
    }

    if (!root.isDirectory())
    {
        Serial.println(
            "[SD] Path is not a directory"
        );

        root.close();

        return;
    }

    File file = root.openNextFile();

    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  [DIR]  ");
        }
        else
        {
            Serial.print("  [FILE] ");
        }

        Serial.print(file.name());

        if (!file.isDirectory())
        {
            Serial.print(" - ");
            Serial.print(file.size());
            Serial.print(" bytes");
        }

        Serial.println();

        file.close();

        file = root.openNextFile();
    }

    root.close();
}