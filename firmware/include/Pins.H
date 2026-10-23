#pragma once

namespace Pins {

// Shared SPI
constexpr int SPI_SCK  = 18;
constexpr int SPI_MISO = 19;
constexpr int SPI_MOSI = 23;

// microSD
constexpr int SD_CS = 5;

// RC522
constexpr int RFID_CS  = 21;
constexpr int RFID_RST = 22;

// MAX98357A
constexpr int I2S_BCLK = 26;
constexpr int I2S_LRC  = 25;
constexpr int I2S_DOUT = 27;

// Rotary encoder
constexpr int ENCODER_CLK = 32;
constexpr int ENCODER_DT  = 33;
constexpr int ENCODER_SW  = 4;

}