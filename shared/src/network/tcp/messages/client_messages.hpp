#ifndef CLIENTMESSAGES_H
#define CLIENTMESSAGES_H

#include "audio_analyzer_message.hpp"

#include <array>
#include <shared_constants.hpp>

namespace SyncBlink
{
    namespace Client
    {
        /**
         * @brief   A set of message types a SyncBlink client will send
         */
        enum MessageType
        {
            MESH_CONNECTION,    // A new node connected to the mesh
            MESH_DISCONNECTION, // A node disconnected
            MESH_UPDATED,       // Answer to the Client::MESH_UPDATE
            SCRIPT_DISTRIBUTED, // Answer to the script distribution
            EXTERNAL_ANALYZER   // Analyzer information by an external analyzer (defined in *SoundAnalyzerSource* enum)
        };

        struct ConnectionMessage
        {
            bool isStation;
            bool isAnalyzer;
            uint64_t nodeId;
            uint64_t parentId;
            uint16_t ledCount;
            uint8_t majorVersion;
            uint8_t minorVersion;
            char nodeLabel[MaxNodeLabelLength];
        };
    }
}

#endif // CLIENTMESSAGES_H