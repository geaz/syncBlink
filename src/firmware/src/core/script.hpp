#ifndef SCRIPT_H
#define SCRIPT_H

#include <LittleFS.h>
#include <string>

namespace SyncBlink
{
    class Script
    {
    public:
        ~Script()
        {
            closeFile();
        }

        File& getScriptFile(bool writable = false)
        {
            if (!_scriptOpen)
            {
                _scriptFile = LittleFS.open(ScriptPath.c_str(), writable ? "w" : "r");
                _scriptOpen = true;
            }
            return _scriptFile;
        }

        File& getBytecodeFile(bool writable = false)
        {
            if (!_bytecodeOpen)
            {
                _bytecodeFile = LittleFS.open(BytecodePath.c_str(), writable ? "w" : "r");
                _bytecodeOpen = true;
            }
            return _bytecodeFile;
        }

        void closeFile()
        {
            if (_scriptOpen) _scriptFile.close();
            if (_bytecodeOpen) _bytecodeFile.close();

            _scriptOpen = false;
            _bytecodeOpen = false;
        }

        bool Exists = false;
        bool IsCompiled = false;

        std::string Name;
        std::string ScriptPath;
        std::string BytecodePath;

    private:
        bool _scriptOpen = false;
        bool _bytecodeOpen = false;

        File _scriptFile;
        File _bytecodeFile;
    };
}

#endif // SCRIPT_H