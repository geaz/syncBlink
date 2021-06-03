#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

namespace SyncBlink
{
    class Script
    {
        public:
            bool Exists;
            std::string Name;
            std::string Content;
    };
}

#endif // SCRIPT_H