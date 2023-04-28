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
        class MeshConnection : public Message
        {
        public:
            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addBytes(package, (void*)&nodeId, sizeof(nodeId));
                addBytes(package, (void*)&isConnected, sizeof(isConnected));
                addBytes(package, (void*)&nodeInfo.isStation, sizeof(nodeInfo.isStation));
                addBytes(package, (void*)&nodeInfo.isAnalyzer, sizeof(nodeInfo.isAnalyzer));
                addBytes(package, (void*)&nodeInfo.isNode, sizeof(nodeInfo.isNode));
                addBytes(package, (void*)&nodeInfo.connectedToMeshWifi, sizeof(nodeInfo.connectedToMeshWifi));
                addBytes(package, (void*)&nodeInfo.parentId, sizeof(nodeInfo.parentId));
                addBytes(package, (void*)&nodeInfo.ledCount, sizeof(nodeInfo.ledCount));
                addBytes(package, (void*)&nodeInfo.majorVersion, sizeof(nodeInfo.majorVersion));
                addBytes(package, (void*)&nodeInfo.minorVersion, sizeof(nodeInfo.minorVersion));
                addStringBytes(package, nodeInfo.nodeLabel);

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadBytes(&package.body[offset], (void*)&nodeId, sizeof(nodeId));
                offset += loadBytes(&package.body[offset], (void*)&isConnected, sizeof(isConnected));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.isStation, sizeof(nodeInfo.isStation));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.isAnalyzer, sizeof(nodeInfo.isAnalyzer));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.isNode, sizeof(nodeInfo.isNode));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.connectedToMeshWifi, sizeof(nodeInfo.connectedToMeshWifi));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.parentId, sizeof(nodeInfo.parentId));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.ledCount, sizeof(nodeInfo.ledCount));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.majorVersion, sizeof(nodeInfo.majorVersion));
                offset += loadBytes(&package.body[offset], (void*)&nodeInfo.minorVersion, sizeof(nodeInfo.minorVersion));
                loadStringBytes(&package.body[offset], nodeInfo.nodeLabel);
            }

            MessageType getMessageType() const override
            {
                return MessageType::MeshConnection;
            }

            uint64_t nodeId;
            bool isConnected;
            NodeInfo nodeInfo;
        };
    }
}

#endif // MESHCONNECTIONMSG_H