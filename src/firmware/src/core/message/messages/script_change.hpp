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

            ScriptChange(std::string scriptName, bool contentChanged = false) : scriptName{scriptName}, contentChanged{contentChanged}
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addBytes(package, (void*)&contentChanged, sizeof(contentChanged));
                addStringBytes(package, scriptName);

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadBytes(&package.body[offset], (void*)&contentChanged, sizeof(contentChanged));
                loadStringBytes(&package.body[offset], scriptName);
            }

            MessageType getMessageType() const override
            {
                return MessageType::ScriptChange;
            }

            std::string scriptName;
            bool contentChanged;
        };
    }
}

#endif // SCRIPTCHANGEMSG_H