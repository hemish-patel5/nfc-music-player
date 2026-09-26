#include <Arduino.h>
#include <SPI.h>

#include "RFIDScanner.h"
#include "SDCardReader.h"

// Shared SPI pins.
constexpr uint8_t SPI_SCK_PIN = 18;
constexpr uint8_t SPI_MISO_PIN = 19;
constexpr uint8_t SPI_MOSI_PIN = 23;

// Each SPI device has a different chip-select pin.
constexpr uint8_t SD_CS_PIN = 5;
constexpr uint8_t RFID_CS_PIN = 21;
constexpr uint8_t RFID_RESET_PIN = 22;

SDCardReader sdCard(SD_CS_PIN, SPI);
RFIDScanner rfidScanner(RFID_CS_PIN, RFID_RESET_PIN);

void printSavedSongs() {
  const std::vector<String> &songs = sdCard.songs();

  Serial.println("MP3 files on the SD card:");
  for (size_t i = 0; i < songs.size(); i++) {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.println(songs[i]);
  }

  if (songs.empty()) {
    Serial.println("No MP3 files found.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Deselect both devices before starting their shared SPI bus.
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  pinMode(RFID_CS_PIN, OUTPUT);
  digitalWrite(RFID_CS_PIN, HIGH);

  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);

  if (sdCard.begin() && sdCard.loadSongs()) {
    printSavedSongs();
  } else {
    Serial.println("Could not read the SD card.");
  }

  if (rfidScanner.begin()) {
    Serial.println("RC522 ready. Place an NFC sticker near the reader.");
  } else {
    Serial.println("Could not communicate with the RC522.");
  }
}

void loop() {
  String stickerUid;

  if (rfidScanner.scan(stickerUid)) {
    Serial.print("NFC sticker UID: ");
    Serial.println(stickerUid);
  }

  delay(50);
}
