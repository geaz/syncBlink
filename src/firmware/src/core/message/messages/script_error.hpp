#ifndef SCRIPTERRORMSG_H
#define SCRIPTERRORMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"
#include "core/script.hpp"

namespace SyncBlink
{
    namespace Messages
    {
        class ScriptError : public Message
        {
        public:
            ScriptError()
            {
            }

            ScriptError(Script script, std::string error) : script{script}, error{error}
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addStringBytes(package, script.Name);
                addStringBytes(package, script.Content);
                addBytes(package, (void*)&script.Exists, sizeof(script.Exists));
                addStringBytes(package, error);

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadStringBytes(&package.body[offset], script.Name);
                offset += loadStringBytes(&package.body[offset], script.Content);
                offset += loadBytes(&package.body[offset], (void*)&script.Exists, sizeof(script.Exists));
                loadStringBytes(&package.body[offset], error);
            }

            MessageType getMessageType() const override
            {
                return MessageType::ScriptError;
            }

            Script script;
            std::string error;
        };
    }
}

#endif // SCRIPTERRORMSG_H