#ifndef MESHUPDATEMSG_H
#define MESHUPDATEMSG_H

#include "core/script.hpp"
#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

#include <cinttypes>

namespace SyncBlink
{
    namespace Messages
    {
        struct MeshUpdate : public Message
        {
            MeshUpdate(){}
            MeshUpdate(Script _script, uint32_t _routeLedCount, uint32_t _routeNodeCount, uint32_t _meshLedCount, uint32_t _meshNodeCount)
                : script{_script}, routeLedCount{_routeLedCount}, routeNodeCount{_routeNodeCount}, meshLedCount{_meshLedCount}, meshNodeCount{_meshNodeCount} {}

            MessageType getMessageType() const { return MessageType::MeshUpdate; }

            Script script;
            uint32_t routeLedCount;
            uint32_t routeNodeCount;
            uint32_t meshLedCount;
            uint32_t meshNodeCount;
        };
    }
}

#endif // MESHUPDATEMSG_H