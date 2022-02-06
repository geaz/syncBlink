#ifndef MESHCONNECTIONMSG_H
#define MESHCONNECTIONMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

#include <string>

namespace SyncBlink
{
    struct NodeInfo
    {
        bool isStation;
        bool isAnalyzer;
        bool isNode;
        bool connectedToMeshWifi;
        uint64_t parentId;
        uint16_t ledCount;
        uint8_t majorVersion;
        uint8_t minorVersion;
        std::string nodeLabel;
    };

    namespace Messages
    {
        struct MeshConnection : public Message
        {
            MeshConnection(){}
            MeshConnection(uint64_t _nodeId, bool _isConnected)
                : nodeId{_nodeId}, isConnected{_isConnected} {}

            MessageType getMessageType() const { return MessageType::MeshConnection; }

            uint64_t nodeId;
            bool isConnected;
            NodeInfo nodeInfo;
        };
    }
}

#endif // MESHCONNECTIONMSG_H