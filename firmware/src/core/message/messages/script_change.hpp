#ifndef SCRIPTCHANGEMSG_H
#define SCRIPTCHANGEMSG_H

#include "core/script.hpp"
#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

namespace SyncBlink
{
    namespace Messages
    {
        class ScriptChange : public Message
        {
        public:
            ScriptChange() {}
            ScriptChange(Script _script) : script{_script} {}

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addStringBytes(package, script.Name);
                addStringBytes(package, script.Content);
                addBytes(package, (void*) &script.Exists, sizeof(script.Exists));

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadStringBytes(&package.body[offset], script.Name);
                offset += loadStringBytes(&package.body[offset], script.Content);
                loadBytes(&package.body[offset], (void*) &script.Exists, sizeof(script.Exists));
            }

            MessageType getMessageType() const override { return MessageType::ScriptChange; }

            Script script;
        };
    }
}

#endif // SCRIPTCHANGEMSG_H