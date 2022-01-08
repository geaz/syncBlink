#include "config.hpp"

#include <LittleFS.h>

namespace SyncBlink
{
    void Config::load()
    {        
        File file = LittleFS.open(ConfigPath.c_str(), "r");
        deserializeJson(Values, file);
        file.close();
    }

    void Config::save()
    {
        File file = LittleFS.open(ConfigPath.c_str(), "w");
        serializeJson(Values, file);
        file.close();
    }
}