#ifndef CLIENTMESSAGES_H
#define CLIENTMESSAGES_H

#include "audio_analyzer_message.hpp"

#include <array>

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
            MOD_DISTRIBUTED,    // Answer to the MOD distribution
            EXTERNAL_ANALYZER   // Analyzer information by an external analyzer (defined in *SoundAnalyzerSource* enum)
        };

        struct ConnectionMessage
        {
            uint64_t clientId;
            uint64_t parentId;
            uint32_t ledCount;
            float firmwareVersion;
        };

        /**
         * @brief   The *Message* is a union struct used by the clients to
         *          send the different kinds of messages to the server.
         */
        struct Message
        {
            uint64_t id;
            MessageType messageType;
            union {
                uint64_t disconnectedClientId;
                ConnectionMessage connectionMessage;
                AudioAnalyzerMessage audioAnalyzerMessage;
            };
        };
    }
}

#endif // CLIENTMESSAGES_H