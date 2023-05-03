#ifndef SCRIPTSOURCE_H
#define SCRIPTSOURCE_H

#include <string>

namespace SyncBlink
{
    class ScriptSource
    {
    public:        
        virtual size_t length() = 0;
        virtual std::string substr(const size_t start, const size_t length) = 0;
        virtual char getChar(const size_t pos) = 0;
    };
}

#endif // SCRIPTSOURCE_H