#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <LittleFS.h>

namespace SyncBlink
{
    class Script
    {
    public:
        ~Script()
        {
            if(Exists) _scriptFile.close();
        }

        File& getFile(bool writable = false)
        {
            if(!_fileOpen)
            {
                _scriptFile = LittleFS.open(Path.c_str(), writable ? "w" : "r");
                _fileOpen = true;
            }            
            return _scriptFile;
        }

        void closeFile()
        {
            if(Exists) _scriptFile.close();
            _fileOpen = false;
        }

        bool Exists = false;
        std::string Name;
        std::string Path;
    
    private:
        bool _fileOpen = false;
        File _scriptFile;
    };
}

#endif // SCRIPT_H