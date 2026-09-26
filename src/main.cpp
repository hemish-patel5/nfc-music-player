#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <SPI.h>

// microSD card pins.
constexpr int SD_SCK_PIN = 18;
constexpr int SD_MISO_PIN = 19;
constexpr int SD_MOSI_PIN = 23;
constexpr int SD_CS_PIN = 5;

// MAX98357A amplifier pins.
constexpr int I2S_LRC_PIN = 25;
constexpr int I2S_BCLK_PIN = 26;
constexpr int I2S_DOUT_PIN = 27;  // Connect this to DIN on the amplifier.

Audio audio;
String songPath;
unsigned long lastRestartAttempt = 0;

// Search this folder and its subfolders for the first MP3 file.
String findFirstMp3(const char *folderPath) {
  File folder = SD.open(folderPath);
  if (!folder || !folder.isDirectory()) {
    folder.close();
    return "";
  }

  File entry = folder.openNextFile();
  while (entry) {
    String path = entry.path();

    if (entry.isDirectory()) {
      entry.close();
      String result = findFirstMp3(path.c_str());
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

void startSong() {
  Serial.printf("Playing: %s\n", songPath.c_str());
  if (!audio.connecttoFS(SD, songPath.c_str())) {
    Serial.println("ERROR: The audio library could not open the MP3.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting SD music test...");

  SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN, SPI, 1000000)) {
    Serial.println("ERROR: Could not read the SD card.");
    return;
  }
  Serial.println("SD card connected.");

  songPath = findFirstMp3("/");
  if (songPath.isEmpty()) {
    Serial.println("ERROR: No MP3 files were found on the SD card.");
    return;
  }

  audio.setPinout(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DOUT_PIN);
  audio.setVolume(21);  // Library range: 0 (silent) to 21 (maximum).
  startSong();
}

void loop() {
  audio.loop();

  // Restart the file after it finishes or if opening it initially failed.
  if (!songPath.isEmpty() && !audio.isRunning() &&
      millis() - lastRestartAttempt >= 1000) {
    lastRestartAttempt = millis();
    startSong();
  }

  delay(1);
}

// Diagnostic messages supplied by ESP32-audioI2S.
void audio_info(const char *message) {
  Serial.print("Audio: ");
  Serial.println(message);
}

void audio_id3data(const char *message) {
  Serial.print("MP3 tag: ");
  Serial.println(message);
}

void audio_eof_mp3(const char *message) {
  Serial.print("Finished: ");
  Serial.println(message);
}
