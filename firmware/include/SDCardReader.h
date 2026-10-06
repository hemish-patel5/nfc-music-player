#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <vector>

class SDCardReader {
 public:
  SDCardReader(uint8_t chipSelectPin, SPIClass &spiBus)
      : chipSelectPin_(chipSelectPin), spiBus_(spiBus) {}

  // Connect to the SD card.
  bool begin() {
    pinMode(chipSelectPin_, OUTPUT);
    digitalWrite(chipSelectPin_, HIGH);
    return SD.begin(chipSelectPin_, spiBus_);
  }

  // Scan the card and save every MP3 path in the songs list.
  bool loadSongs() {
    songs_.clear();

    File root = SD.open("/");
    if (!root || !root.isDirectory()) {
      root.close();
      return false;
    }

    scanDirectory(root);
    root.close();
    return true;
  }

  // Give other parts of the program read-only access to the song list.
  const std::vector<String> &songs() const {
    return songs_;
  }

 private:
  uint8_t chipSelectPin_;
  SPIClass &spiBus_;
  std::vector<String> songs_;

  bool isMp3File(String fileName) {
    fileName.toLowerCase();
    return fileName.endsWith(".mp3");
  }

  // Search a directory and recursively search any folders inside it.
  void scanDirectory(File directory) {
    File entry = directory.openNextFile();

    while (entry) {
      if (entry.isDirectory()) {
        scanDirectory(entry);
      } else if (isMp3File(entry.name())) {
        songs_.push_back(String(entry.path()));
      }

      entry.close();
      entry = directory.openNextFile();
    }
  }
};
