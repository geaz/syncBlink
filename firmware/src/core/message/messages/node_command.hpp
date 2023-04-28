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
            Rename
        };

        struct CommandInfo
        {
            std::string stringInfo1;
        };

        class NodeCommand : public Message
        {
        public:
            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;                
                addBytes(package, (void*)&recipientId, sizeof(recipientId));
                addBytes(package, (void*)&commandType, sizeof(commandType));
                addStringBytes(package, commandInfo.stringInfo1);

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadBytes(&package.body[offset], (void*)&recipientId, sizeof(recipientId));
                offset += loadBytes(&package.body[offset], (void*)&commandType, sizeof(commandType));
                loadStringBytes(&package.body[offset], commandInfo.stringInfo1);
            }

            MessageType getMessageType() const override
            {
                return MessageType::NodeCommand;
            }
                        
            CommandInfo commandInfo;
            NodeCommandType commandType;
        };
    }
}

#endif // NODECOMMANDMSG_H