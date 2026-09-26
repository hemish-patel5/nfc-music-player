#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

// microSD connections.
constexpr uint8_t SD_CS_PIN = 5;
constexpr uint8_t SD_SCK_PIN = 18;
constexpr uint8_t SD_MISO_PIN = 19;
constexpr uint8_t SD_MOSI_PIN = 23;

// Keep the RC522 deselected because it shares the SPI bus.
constexpr uint8_t RFID_CS_PIN = 21;

// MAX98357A connections.
constexpr uint8_t I2S_LRC_PIN = 25;
constexpr uint8_t I2S_BCLK_PIN = 26;
constexpr uint8_t I2S_DATA_PIN = 27;

String songPath;
AudioFileSourceSD audioFile;
AudioOutputI2S audioOutput;
AudioGeneratorMP3 mp3Decoder;

bool findFirstMp3(File directory) {
  File entry = directory.openNextFile();

  while (entry) {
    bool foundSong = false;

    if (entry.isDirectory()) {
      foundSong = findFirstMp3(entry);
    } else {
      String fileName = entry.name();
      fileName.toLowerCase();

      if (fileName.endsWith(".mp3")) {
        songPath = entry.path();
        foundSong = true;
      }
    }

    entry.close();

    if (foundSong) {
      return true;
    }

    entry = directory.openNextFile();
  }

  return false;
}

bool startSong() {
  audioFile.close();

  if (!audioFile.open(songPath.c_str())) {
    Serial.print("Could not open: ");
    Serial.println(songPath);
    return false;
  }

  Serial.print("Playing: ");
  Serial.println(songPath);

  return mp3Decoder.begin(&audioFile, &audioOutput);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  pinMode(RFID_CS_PIN, OUTPUT);
  digitalWrite(RFID_CS_PIN, HIGH);

  SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

  if (!SD.begin(SD_CS_PIN, SPI)) {
    Serial.println("SD card initialization failed.");
    return;
  }

  File root = SD.open("/");
  if (!root || !findFirstMp3(root)) {
    Serial.println("No MP3 file found on the SD card.");
    root.close();
    return;
  }
  root.close();

  if (!audioOutput.SetPinout(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DATA_PIN)) {
    Serial.println("Could not configure the I2S pins.");
    return;
  }

  // 1.0 is full digital volume without intentionally clipping the audio.
  audioOutput.SetGain(1.0F);

  if (!startSong()) {
    Serial.println("Could not start MP3 playback.");
  }
}

void loop() {
  if (songPath.isEmpty()) {
    delay(1000);
    return;
  }

  if (mp3Decoder.isRunning()) {
    if (!mp3Decoder.loop()) {
      mp3Decoder.stop();
    }
  } else {
    // The song ended (or stopped), so open it again from the beginning.
    delay(50);
    startSong();
  }
}
