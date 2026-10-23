#pragma once

namespace AppConfig{

constexpr int DEFAULT_VOLUME = 10;
constexpr int MIN_VOLUME = 0;
constexpr int MAX_VOLUME = 20;

constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;

constexpr unsigned long NFC_SCAN_INTERVAL_MS = 100;

constexpr unsigned long CLOUD_CHECK_INTERVAL_MS =
    60 * 1000;

constexpr const char* CONFIG_FILE =
    "/config/config.json";

constexpr const char* MUSIC_DIRECTORY =
    "/music";


}