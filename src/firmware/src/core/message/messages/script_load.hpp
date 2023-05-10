#ifndef SCRIPTLOADMSG_H
#define SCRIPTLOADMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

namespace SyncBlink
{
    namespace Messages
    {
        class ScriptLoad : public Message
        {
        public:
            ScriptLoad()
            {
            }

            ScriptLoad(std::string scriptName) : scriptName{scriptName}
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
                loadStringBytes(&package.body[offset], scriptName);
            }

            MessageType getMessageType() const override
            {
                return MessageType::ScriptLoad;
            }

            std::string scriptName;
        };
    }
}

#endif // SCRIPTLOADMSG_H