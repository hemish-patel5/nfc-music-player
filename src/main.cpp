#include <Arduino.h>
#include <cmath>
#include <driver/i2s.h>

// MAX98357A connections.
constexpr int I2S_LRC_PIN = 25;
constexpr int I2S_BCLK_PIN = 26;
constexpr int I2S_DATA_PIN = 27;

constexpr i2s_port_t I2S_PORT = I2S_NUM_0;
constexpr uint32_t SAMPLE_RATE = 44100;
constexpr size_t FRAMES_PER_BUFFER = 128;
constexpr float FULL_CIRCLE_RADIANS = 6.28318530718F;

bool startSpeaker() {
  i2s_config_t config = {};
  config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX);
  config.sample_rate = SAMPLE_RATE;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  config.dma_buf_count = 8;
  config.dma_buf_len = FRAMES_PER_BUFFER;
  config.use_apll = false;
  config.tx_desc_auto_clear = true;
  config.fixed_mclk = 0;

  i2s_pin_config_t pins = {};
  pins.mck_io_num = I2S_PIN_NO_CHANGE;
  pins.bck_io_num = I2S_BCLK_PIN;
  pins.ws_io_num = I2S_LRC_PIN;
  pins.data_out_num = I2S_DATA_PIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;

  if (i2s_driver_install(I2S_PORT, &config, 0, nullptr) != ESP_OK) {
    return false;
  }

  if (i2s_set_pin(I2S_PORT, &pins) != ESP_OK) {
    i2s_driver_uninstall(I2S_PORT);
    return false;
  }

  i2s_zero_dma_buffer(I2S_PORT);
  return true;
}

void playTone(float frequency, uint32_t durationMs) {
  // Each frame contains a left and right 16-bit sample.
  int16_t samples[FRAMES_PER_BUFFER * 2];
  uint32_t framesRemaining = (SAMPLE_RATE * durationMs) / 1000;
  float phase = 0.0F;
  const float phaseStep = FULL_CIRCLE_RADIANS * frequency / SAMPLE_RATE;

  while (framesRemaining > 0) {
    const size_t frameCount =
        framesRemaining < FRAMES_PER_BUFFER ? framesRemaining
                                             : FRAMES_PER_BUFFER;

    for (size_t frame = 0; frame < frameCount; frame++) {
      // A moderate volume helps protect the speaker during testing.
      const int16_t sample = static_cast<int16_t>(sinf(phase) * 6000);
      samples[frame * 2] = sample;
      samples[frame * 2 + 1] = sample;

      phase += phaseStep;
      if (phase >= FULL_CIRCLE_RADIANS) {
        phase -= FULL_CIRCLE_RADIANS;
      }
    }

    size_t bytesWritten = 0;
    i2s_write(I2S_PORT, samples, frameCount * 2 * sizeof(int16_t),
              &bytesWritten, portMAX_DELAY);
    framesRemaining -= frameCount;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("MAX98357A speaker test");

  if (!startSpeaker()) {
    Serial.println("Could not start I2S.");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("Speaker ready.");
}

void loop() {
  Serial.println("Playing test tones...");

  playTone(262.0F, 500);  // C
  playTone(330.0F, 500);  // E
  playTone(392.0F, 500);  // G

  i2s_zero_dma_buffer(I2S_PORT);
  Serial.println("Test finished. Repeating in 3 seconds.");
  delay(3000);
}
