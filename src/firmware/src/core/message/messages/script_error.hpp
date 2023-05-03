#ifndef SCRIPTERRORMSG_H
#define SCRIPTERRORMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

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

            ScriptError(std::string scriptName, std::string error) : scriptName{scriptName}, error{error}
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addStringBytes(package, scriptName);
                addStringBytes(package, error);

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadStringBytes(&package.body[offset], scriptName);
                loadStringBytes(&package.body[offset], error);
            }

            MessageType getMessageType() const override
            {
                return MessageType::ScriptError;
            }

            std::string scriptName;
            std::string error;
        };
    }
}

#endif // SCRIPTERRORMSG_H