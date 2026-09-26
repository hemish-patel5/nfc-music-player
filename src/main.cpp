#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// SPI pins used by the microSD card.
constexpr uint8_t SD_CS_PIN = 5;
constexpr uint8_t SD_SCK_PIN = 18;
constexpr uint8_t SD_MISO_PIN = 19;
constexpr uint8_t SD_MOSI_PIN = 23;

// The RC522 shares the SPI bus but has its own chip-select pin.
constexpr uint8_t RFID_CS_PIN = 21;

unsigned int musicFileCount = 0;

// Return true when a filename has a supported music-file extension.
bool isMusicFile(String fileName) {
  fileName.toLowerCase();

  return fileName.endsWith(".mp3") || fileName.endsWith(".wav") ||
         fileName.endsWith(".aac") || fileName.endsWith(".m4a") ||
         fileName.endsWith(".flac") || fileName.endsWith(".ogg");
}

// Look through this directory and every directory inside it.
void printMusicFiles(File directory) {
  File entry = directory.openNextFile();

  while (entry) {
    if (entry.isDirectory()) {
      printMusicFiles(entry);
    } else if (isMusicFile(entry.name())) {
      musicFileCount++;
      Serial.print(musicFileCount);
      Serial.print(". ");
      Serial.print(entry.path());
      Serial.print(" (");
      Serial.print(entry.size());
      Serial.println(" bytes)");
    }

    entry.close();
    entry = directory.openNextFile();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // The RC522 shares the SPI bus, so keep it deselected.
  pinMode(RFID_CS_PIN, OUTPUT);
  digitalWrite(RFID_CS_PIN, HIGH);

  SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

  if (!SD.begin(SD_CS_PIN, SPI)) {
    Serial.println("SD card initialization failed.");
    return;
  }

  File root = SD.open("/");
  if (!root || !root.isDirectory()) {
    Serial.println("Could not open the SD card root directory.");
    return;
  }

  Serial.println("Music files on the SD card:");
  printMusicFiles(root);
  root.close();

  if (musicFileCount == 0) {
    Serial.println("No music files found.");
  } else {
    Serial.print("Total music files: ");
    Serial.println(musicFileCount);
  }
}

void loop() {}
