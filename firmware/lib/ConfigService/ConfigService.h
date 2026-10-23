#pragma once

#include <Arduino.h>
#include "SdStorage.h"


struct DiscInfo
{
    int discId;
    String uid;
    String title;
    String file;
};


class ConfigService
{
public:
    ConfigService(SdStorage& storage);

    bool load(
        const String& path =
            "/config/config.json"
    );

    bool findByUid(
        const String& uid,
        DiscInfo& result
    ) const;

    int getVersion() const;

    int getDiscCount() const;


private:
    static constexpr int MAX_DISCS = 20;

    SdStorage& _storage;

    DiscInfo _discs[MAX_DISCS];

    int _discCount;
    int _version;
};