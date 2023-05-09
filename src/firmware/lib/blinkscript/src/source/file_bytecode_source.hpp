#ifndef ESP8266
#ifndef FILEBYTECODESOURCE_H
#define FILEBYTECODESOURCE_H

#include "bytecode_source.hpp"

#include <string>
#include <fstream>
#include <filesystem>

namespace SyncBlink
{
    class FileByteCodeSource : public ByteCodeSource
    {
    public:
        FileByteCodeSource(const std::string& filePath)
        {
            _file = std::ifstream(filePath, std::ifstream::binary);
        }

        ~FileByteCodeSource()
        {
            _file.close();
        }

        uint8_t getByte(const size_t pos) override
        { 
            _file.seekg(pos);

            uint8_t byte[1];
            _file.read((char*)&byte[0], 1);

            return byte[0];
        };

    private:
        std::ifstream _file;
    };
}

#endif // FILEBYTECODESOURCE_H
#endif