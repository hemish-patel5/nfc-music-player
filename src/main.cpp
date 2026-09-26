#include <Arduino.h>
#include <cmath>
#include <driver/i2s.h>
#include <esp_err.h>

constexpr i2s_port_t I2S_PORT = I2S_NUM_0;
constexpr int LRC_PIN = 25;
constexpr int BCLK_PIN = 26;
constexpr int DIN_PIN = 27;

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr float TONE_FREQUENCY = 1000.0F;
constexpr float FULL_CIRCLE_RADIANS = 6.28318530718F;
constexpr size_t FRAME_COUNT = 128;

bool checkResult(const char *operation, esp_err_t result) {
  Serial.print(operation);
  Serial.print(": ");
  Serial.println(esp_err_to_name(result));
  return result == ESP_OK;
}

bool startI2s() {
  i2s_config_t config = {};
  config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX);
  config.sample_rate = SAMPLE_RATE;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  config.dma_buf_count = 8;
  config.dma_buf_len = FRAME_COUNT;
  config.use_apll = false;
  config.tx_desc_auto_clear = true;

  i2s_pin_config_t pins = {};
  pins.mck_io_num = I2S_PIN_NO_CHANGE;
  pins.bck_io_num = BCLK_PIN;
  pins.ws_io_num = LRC_PIN;
  pins.data_out_num = DIN_PIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;

  esp_err_t result = i2s_driver_install(I2S_PORT, &config, 0, nullptr);
  if (!checkResult("Install I2S driver", result)) {
    return false;
  }

  result = i2s_set_pin(I2S_PORT, &pins);
  if (!checkResult("Configure I2S pins", result)) {
    return false;
  }

  result = i2s_set_clk(I2S_PORT, SAMPLE_RATE,
                       I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
  return checkResult("Set I2S clock", result);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("MAX98357A diagnostic");
  Serial.println("LRC=25, BCLK=26, DIN=27");

  if (!startI2s()) {
    Serial.println("I2S setup failed.");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("I2S setup passed. Starting continuous 1 kHz tone.");
}

void loop() {
  int16_t samples[FRAME_COUNT * 2];
  static float phase = 0.0F;
  const float phaseStep =
      FULL_CIRCLE_RADIANS * TONE_FREQUENCY / SAMPLE_RATE;

  for (size_t frame = 0; frame < FRAME_COUNT; frame++) {
    const int16_t sample = static_cast<int16_t>(sinf(phase) * 12000);
    samples[frame * 2] = sample;
    samples[frame * 2 + 1] = sample;

    phase += phaseStep;
    if (phase >= FULL_CIRCLE_RADIANS) {
      phase -= FULL_CIRCLE_RADIANS;
    }
  }

  size_t bytesWritten = 0;
  const esp_err_t result =
      i2s_write(I2S_PORT, samples, sizeof(samples), &bytesWritten,
                portMAX_DELAY);

  static uint32_t lastReport = 0;
  if (millis() - lastReport >= 1000) {
    Serial.print("Audio write: ");
    Serial.print(esp_err_to_name(result));
    Serial.print(", bytes: ");
    Serial.println(bytesWritten);
    lastReport = millis();
  }
}
