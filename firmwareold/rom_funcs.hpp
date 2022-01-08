#ifndef ROMREADER_H
#define ROMREADER_H

#include <EEPROM.h>
#include <string>
#include <cinttypes>

namespace SyncBlink
{
    inline void writeRomString(uint8_t start, char *stringArr, uint8_t length)
    {
        for (uint8_t i = 0; i < length; i++) EEPROM.write(start + i, stringArr[i]);
        EEPROM.commit();
    }

    inline std::string readRomString(uint8_t start, uint8_t maxLength)
    {
        int i = start;
        char chars[maxLength];

        for(int i = 0; i < maxLength; i++){
            chars[i] = '\0';
        }

        char curChar = char(EEPROM.read(start));
        while (i < start + maxLength && curChar != '\0' && curChar >= 32 && curChar <= 126)
        {
            curChar = char(EEPROM.read(i));
            chars[i - start] = curChar;
            i++;
        }
        
        return std::string(chars);
    }

    inline int8_t readRomInt8(uint8_t start)
    {
        int8_t nr = 0;
        for(int i = start; i < start + 4; i++)
        {
            nr += EEPROM.read(i) << (i*8);
        }
        return nr;
    }
}

#endif // ROMREADER_H