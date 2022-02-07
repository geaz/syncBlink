#ifndef MESHUPDATEMSG_H
#define MESHUPDATEMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"
#include "core/script.hpp"

#include <cinttypes>

namespace SyncBlink
{
    namespace Messages
    {
        class MeshUpdate : public Message
        {
        public:
            MeshUpdate()
            {
            }
            
            MeshUpdate(Script _script, uint32_t _routeLedCount, uint32_t _routeNodeCount, uint32_t _meshLedCount,
                       uint32_t _meshNodeCount)
                : script{_script}, routeLedCount{_routeLedCount}, routeNodeCount{_routeNodeCount},
                  meshLedCount{_meshLedCount}, meshNodeCount{_meshNodeCount}
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addStringBytes(package, script.Name);
                addStringBytes(package, script.Content);
                addBytes(package, (void*)&script.Exists, sizeof(script.Exists));
                addBytes(package, (void*)&routeLedCount, sizeof(routeLedCount));
                addBytes(package, (void*)&routeNodeCount, sizeof(routeNodeCount));
                addBytes(package, (void*)&meshLedCount, sizeof(meshLedCount));
                addBytes(package, (void*)&meshNodeCount, sizeof(meshNodeCount));

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadStringBytes(&package.body[offset], script.Name);
                offset += loadStringBytes(&package.body[offset], script.Content);
                offset += loadBytes(&package.body[offset], (void*)&script.Exists, sizeof(script.Exists));
                offset += loadBytes(&package.body[offset], (void*)&routeLedCount, sizeof(routeLedCount));
                offset += loadBytes(&package.body[offset], (void*)&routeNodeCount, sizeof(routeNodeCount));
                offset += loadBytes(&package.body[offset], (void*)&meshLedCount, sizeof(meshLedCount));
                loadBytes(&package.body[offset], (void*)&meshNodeCount, sizeof(meshNodeCount));
            }

            MessageType getMessageType() const override
            {
                return MessageType::MeshUpdate;
            }

            Script script;
            uint32_t routeLedCount;
            uint32_t routeNodeCount;
            uint32_t meshLedCount;
            uint32_t meshNodeCount;
        };
    }
}

#endif // MESHUPDATEMSG_H