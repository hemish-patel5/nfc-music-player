#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_PORT I2S_NUM_0

#define BCLK_PIN 26
#define LRC_PIN  25
#define DIN_PIN  27

void setup() {
    Serial.begin(115200);

    i2s_config_t config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 22050,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pins = {
        .bck_io_num = BCLK_PIN,
        .ws_io_num = LRC_PIN,
        .data_out_num = DIN_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_PORT, &config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pins);

    Serial.println("I2S test started");
}

void loop() {
    static int16_t buffer[512];

    for (int i = 0; i < 512; i += 2) {
        int16_t sample = ((i / 32) % 2) ? 12000 : -12000;

        buffer[i] = sample;
        buffer[i + 1] = sample;
    }

    size_t written;
    i2s_write(
        I2S_PORT,
        buffer,
        sizeof(buffer),
        &written,
        portMAX_DELAY
    );
}