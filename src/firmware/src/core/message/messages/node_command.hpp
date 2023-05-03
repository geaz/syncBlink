#ifndef NODECOMMANDMSG_H
#define NODECOMMANDMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

#include <string>

namespace SyncBlink
{
    namespace Messages
    {
        enum NodeCommandType
        {
            Ping,
            Rename,
            WifiChange,
            ScriptUpdate,
            ScriptUpdated
        };

        struct CommandInfo
        {
            bool flag;
            std::string stringInfo1;
            std::string stringInfo2;
        };

        class NodeCommand : public Message
        {
        public:
            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addBytes(package, (void*)&recipientId, sizeof(recipientId));
                addBytes(package, (void*)&commandType, sizeof(commandType));
                addBytes(package, (void*)&commandInfo.flag, sizeof(commandInfo.flag));
                addStringBytes(package, commandInfo.stringInfo1);
                addStringBytes(package, commandInfo.stringInfo2);

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadBytes(&package.body[offset], (void*)&recipientId, sizeof(recipientId));
                offset += loadBytes(&package.body[offset], (void*)&commandType, sizeof(commandType));
                offset += loadBytes(&package.body[offset], (void*)&commandInfo.flag, sizeof(commandInfo.flag));
                offset += loadStringBytes(&package.body[offset], commandInfo.stringInfo1);
                loadStringBytes(&package.body[offset], commandInfo.stringInfo2);
            }

            MessageType getMessageType() const override
            {
                return MessageType::NodeCommand;
            }

            uint64_t recipientId = 0;
            CommandInfo commandInfo;
            NodeCommandType commandType;
        };
    }
}

#endif // NODECOMMANDMSG_H