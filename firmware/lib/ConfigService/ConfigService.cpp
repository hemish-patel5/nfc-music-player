#include "ConfigService.h"

#include <ArduinoJson.h>


ConfigService::ConfigService(
    SdStorage& storage
)
    : _storage(storage),
      _discCount(0),
      _version(0)
{
}


bool ConfigService::load(
    const String& path
)
{
    Serial.println("[CONFIG] Loading configuration...");

    String json;

    if (!_storage.readFile(path, json))
    {
        Serial.println(
            "[CONFIG] ERROR: Could not read config"
        );

        return false;
    }


    JsonDocument document;

    DeserializationError error =
        deserializeJson(document, json);

    if (error)
    {
        Serial.print(
            "[CONFIG] JSON error: "
        );

        Serial.println(error.c_str());

        return false;
    }


    _version =
        document["version"] | 0;


    JsonArray discs =
        document["discs"].as<JsonArray>();

    _discCount = 0;


    for (JsonObject disc : discs)
    {
        if (_discCount >= MAX_DISCS)
        {
            Serial.println(
                "[CONFIG] Maximum disc count reached"
            );

            break;
        }


        DiscInfo& info =
            _discs[_discCount];


        info.discId =
            disc["disc_id"] | 0;

        info.uid =
            disc["uid"] | "";

        info.title =
            disc["title"] | "";

        info.file =
            disc["file"] | "";


        // Normalise UID
        info.uid.toUpperCase();


        _discCount++;
    }


    Serial.print(
        "[CONFIG] Version: "
    );

    Serial.println(_version);


    Serial.print(
        "[CONFIG] Loaded discs: "
    );

    Serial.println(_discCount);


    return true;
}


bool ConfigService::findByUid(
    const String& uid,
    DiscInfo& result
) const
{
    String searchUid = uid;

    searchUid.toUpperCase();


    for (int i = 0; i < _discCount; i++)
    {
        if (_discs[i].uid == searchUid)
        {
            result = _discs[i];

            return true;
        }
    }


    return false;
}


int ConfigService::getVersion() const
{
    return _version;
}


int ConfigService::getDiscCount() const
{
    return _discCount;
}