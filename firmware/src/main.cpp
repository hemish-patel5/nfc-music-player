#include <Arduino.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <SD.h>
#include <SPI.h>

#include "RFIDScanner.h"

// Shared SPI bus: microSD module and RC522.
constexpr uint8_t SPI_SCK_PIN = 4;
constexpr uint8_t SPI_MISO_PIN = 5;
constexpr uint8_t SPI_MOSI_PIN = 6;

// microSD module.
constexpr uint8_t SD_CS_PIN = 7;

// MAX98357A I2S amplifier.
constexpr uint8_t I2S_LRC_PIN = 0;
constexpr uint8_t I2S_BCLK_PIN = 1;
constexpr uint8_t I2S_DOUT_PIN = 3;  // Connects to DIN on the amplifier.

// RC522 RFID reader.
constexpr uint8_t RFID_CS_PIN = 10;  // Labelled SDA or SS on the RC522.
constexpr uint8_t RFID_RST_PIN = 20;

constexpr uint32_t SD_SPI_FREQUENCY = 1000000;
constexpr uint32_t SONG_RESTART_DELAY_MS = 1000;
constexpr uint32_t RFID_SCAN_INTERVAL_MS = 100;

RFIDScanner rfid(RFID_CS_PIN, RFID_RST_PIN);
AudioGeneratorMP3 mp3;
AudioOutputI2S audioOutput;
AudioFileSourceSD *audioFile = nullptr;

String songPath;
uint32_t lastRestartAttempt = 0;
uint32_t lastRfidScan = 0;

// Search a folder and its subfolders for the first MP3 file.
String findFirstMp3(const char *folderPath) {
  File folder = SD.open(folderPath);
  if (!folder || !folder.isDirectory()) {
    folder.close();
    return "";
  }

  File entry = folder.openNextFile();
  while (entry) {
    const String path = entry.path();

    if (entry.isDirectory()) {
      entry.close();
      const String result = findFirstMp3(path.c_str());
      if (!result.isEmpty()) {
        folder.close();
        return result;
      }
    } else {
      String lowercasePath = path;
      lowercasePath.toLowerCase();
      entry.close();

      if (lowercasePath.endsWith(".mp3")) {
        folder.close();
        return path;
      }
    }

    entry = folder.openNextFile();
  }

  folder.close();
  return "";
}

void closeAudioFile() {
  if (audioFile == nullptr) {
    return;
  }

  audioFile->close();
  delete audioFile;
  audioFile = nullptr;
}

void startSong() {
  if (songPath.isEmpty()) {
    return;
  }

  if (mp3.isRunning()) {
    mp3.stop();
  }
  closeAudioFile();

  Serial.printf("Playing: %s\n", songPath.c_str());
  audioFile = new AudioFileSourceSD(songPath.c_str());

  if (audioFile == nullptr || !audioFile->isOpen()) {
    Serial.println("ERROR: Could not open the MP3 file.");
    closeAudioFile();
    return;
  }

  if (!mp3.begin(audioFile, &audioOutput)) {
    Serial.println("ERROR: Could not start the MP3 decoder.");
    closeAudioFile();
  }
}

void initializeSharedSpiBus() {
  // Keep both SPI devices deselected while configuring the shared bus.
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  pinMode(RFID_CS_PIN, OUTPUT);
  digitalWrite(RFID_CS_PIN, HIGH);

  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
}

void initializeSdCard() {
  if (!SD.begin(SD_CS_PIN, SPI, SD_SPI_FREQUENCY)) {
    Serial.println("ERROR: Could not read the SD card.");
    return;
  }

  Serial.println("SD card connected.");
  songPath = findFirstMp3("/");

  if (songPath.isEmpty()) {
    Serial.println("ERROR: No MP3 files were found on the SD card.");
  } else {
    Serial.printf("Found MP3: %s\n", songPath.c_str());
  }
}

void initializeAudio() {
  if (!audioOutput.SetPinout(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DOUT_PIN)) {
    Serial.println("ERROR: Could not configure the I2S pins.");
    return;
  }

  audioOutput.SetGain(1.0F);
  startSong();
}

void initializeRfid() {
  if (rfid.begin()) {
    Serial.println("RC522 connected. Scan an RFID sticker.");
  } else {
    Serial.println("ERROR: RC522 was not detected.");
  }
}

void updateAudio() {
  if (mp3.isRunning()) {
    if (!mp3.loop()) {
      mp3.stop();
      closeAudioFile();
      lastRestartAttempt = millis();
      Serial.println("Song finished. Restarting shortly...");
    }
    return;
  }

  if (!songPath.isEmpty() &&
      millis() - lastRestartAttempt >= SONG_RESTART_DELAY_MS) {
    lastRestartAttempt = millis();
    startSong();
  }
}

void scanRfid() {
  if (millis() - lastRfidScan < RFID_SCAN_INTERVAL_MS) {
    return;
  }
  lastRfidScan = millis();

  String uid;
  if (rfid.scan(uid)) {
    Serial.printf("RFID UID: %s\n", uid.c_str());
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting ESP32-C3 RFID music player...");

  initializeSharedSpiBus();
  initializeSdCard();
  initializeRfid();
  initializeAudio();
}

void loop() {
  // Audio decoding must run as often as possible to prevent dropouts.
  updateAudio();
  scanRfid();
  delay(1);
}
