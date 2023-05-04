#ifdef ESP8266
#ifndef ESPFILESCRIPTSOURCE_H
#define ESPFILESCRIPTSOURCE_H

#include "script_source.hpp"

#include <string>
#include <LittleFS.h>

namespace SyncBlink
{
    class EspFileScriptSource : public ScriptSource
    {
    public:
        EspFileScriptSource(const std::string& filePath)
        {
            _file = LittleFS.open(filePath.c_str(), "r");
        }

        ~EspFileScriptSource()
        {
            _file.close();
        }

        size_t length() { return _file.size(); };
        
        std::string substr(const size_t start, const size_t length)
        { 
            _file.seek(start);
            char buffer[length];
            _file.readBytes(buffer, length);

            return std::string(buffer, length);
        };

        char getChar(const size_t pos) override
        { 
            _file.seek(pos);
            return _file.read(); 
        };

    private:
        File _file;
    };
}

#endif // ESPFILESCRIPTSOURCE_H
#endif