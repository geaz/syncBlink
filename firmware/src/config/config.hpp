#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>

namespace SyncBlink
{
    static const std::string ConfigPath = "config.json";

    class Config
    {
    public:
        void load();
        void save();

        StaticJsonDocument<512> Values;
    };
}

#endif // CONFIG_H