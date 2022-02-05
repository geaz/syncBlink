#ifndef MESHCONNECTIONEVENT_H
#define MESHCONNECTIONEVENT_H

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

    namespace Events
    {
        struct MeshConnectionEvent
        {
            uint64_t nodeId;
            bool isConnected;
            NodeInfo nodeInfo;
        };
    }
}

#endif // MESHCONNECTIONEVENT_H