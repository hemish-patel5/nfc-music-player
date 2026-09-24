#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

constexpr uint8_t SD_CS = 5;
constexpr uint8_t SD_SCK = 18;
constexpr uint8_t SD_MISO = 19;
constexpr uint8_t SD_MOSI = 23;
constexpr uint8_t RFID_CS = 21;

void listFiles(File directory, uint8_t depth = 0) {
  File entry = directory.openNextFile();

  while (entry) {
    for (uint8_t i = 0; i < depth; i++) {
      Serial.print("  ");
    }

    if (entry.isDirectory()) {
      Serial.print("[DIR]  ");
      Serial.println(entry.name());
      listFiles(entry, depth + 1);
    } else {
      Serial.print("[FILE] ");
      Serial.print(entry.name());
      Serial.print("  ");
      Serial.print(entry.size());
      Serial.println(" bytes");
    }

    entry.close();
    entry = directory.openNextFile();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // The RC522 shares the SPI bus, so keep it deselected.
  pinMode(RFID_CS, OUTPUT);
  digitalWrite(RFID_CS, HIGH);

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, SPI)) {
    Serial.println("SD card initialization failed.");
    return;
  }

  File root = SD.open("/");
  if (!root || !root.isDirectory()) {
    Serial.println("Could not open the SD card root directory.");
    return;
  }

  Serial.println("SD card contents:");
  listFiles(root);
  root.close();
}

void loop() {}
