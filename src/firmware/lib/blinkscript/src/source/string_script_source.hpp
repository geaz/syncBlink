#ifndef STRINGBLINKSCRIPTSOURCE_H
#define STRINGBLINKSCRIPTSOURCE_H

#include "script_source.hpp"

#include <string>

namespace SyncBlink
{
    class StringScriptSource : public ScriptSource
    {
    public:
        StringScriptSource(const std::string& source) : _source(source)
        {
        }

        size_t length() override
        {
            return _source.length();
        };
        std::string substr(const size_t start, const size_t length) override
        {
            return _source.substr(start, length);
        };
        char getChar(const size_t pos) override
        {
            return _source[pos];
        };

    private:
        const std::string _source;
    };
}

#endif // STRINGBLINKSCRIPTSOURCE_H