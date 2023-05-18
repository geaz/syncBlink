#ifdef ESP8266
#ifndef ESPFILESCRIPTSOURCE_H
#define ESPFILESCRIPTSOURCE_H

#include "bytecode_source.hpp"

#include <LittleFS.h>
#include <string>

namespace SyncBlink
{
    class EspFileBytecodeSource : public ByteCodeSource
    {
    public:
        EspFileBytecodeSource(const std::string& filePath)
        {
            _file = LittleFS.open(filePath.c_str(), "r");
        }

        ~EspFileBytecodeSource()
        {
            _file.close();
        }

        uint8_t getByte(const size_t pos) override
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