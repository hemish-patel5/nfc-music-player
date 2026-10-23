#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define SPI_SCK   18
#define SPI_MISO  19
#define SPI_MOSI  23

#define SD_CS     5
#define RFID_CS   21

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("======================");
    Serial.println(" SHARED SPI SD TEST");
    Serial.println("======================");


    // Both devices deselected
    pinMode(SD_CS, OUTPUT);
    pinMode(RFID_CS, OUTPUT);

    digitalWrite(SD_CS, HIGH);
    digitalWrite(RFID_CS, HIGH);

    delay(100);


    // Shared SPI bus
    SPI.begin(
        SPI_SCK,
        SPI_MISO,
        SPI_MOSI
    );

    delay(100);


    Serial.println("Mounting SD...");


    if (!SD.begin(
        SD_CS,
        SPI,
        4000000
    ))
    {
        Serial.println(
            "SD MOUNT FAILED"
        );

        return;
    }


    Serial.println(
        "SD MOUNT SUCCESS"
    );


    Serial.print("Card size: ");

    Serial.print(
        SD.cardSize() /
        (1024ULL * 1024ULL)
    );

    Serial.println(" MB");


    Serial.println();
    Serial.println("Files in /music:");


    File directory =
        SD.open("/music");


    if (!directory)
    {
        Serial.println(
            "Could not open /music"
        );

        return;
    }


    File file =
        directory.openNextFile();


    while (file)
    {
        Serial.print("  ");

        Serial.print(
            file.name()
        );

        Serial.print(" - ");

        Serial.print(
            file.size()
        );

        Serial.println(" bytes");


        file =
            directory.openNextFile();
    }


    Serial.println();
    Serial.println("Test complete.");
}


void loop()
{
}