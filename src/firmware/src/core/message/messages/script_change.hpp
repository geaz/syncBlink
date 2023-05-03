#ifndef SCRIPTCHANGEMSG_H
#define SCRIPTCHANGEMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

namespace SyncBlink
{
    namespace Messages
    {
        class ScriptChange : public Message
        {
        public:
            ScriptChange()
            {
            }

            ScriptChange(std::string scriptName) : scriptName{scriptName}
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addStringBytes(package, scriptName);

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadStringBytes(&package.body[offset], scriptName);
            }

            MessageType getMessageType() const override
            {
                return MessageType::ScriptChange;
            }

            std::string scriptName;
        };
    }
}

#endif // SCRIPTCHANGEMSG_H