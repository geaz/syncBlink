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
            NONE,             // A "null" message type
            MESH_CONNECTION,  // A new node connected to the mesh
            MESH_COUNTED,     // Answer to the Server::MESH_COUNT_REQUEST
            MESH_UPDATED,     // Answer to the Client::MESH_UPDATE
            MOD_DISTRIBUTED,  // Answer to the MOD distribution
            EXTERNAL_ANALYZER // Analyzer information by an external analyzer (defined in *SoundAnalyzerSource* enum)
        };

        /**
         * @brief   This message is the answer to the Server::MESH_COUNT_REQUEST
         *          It contains the total amount of LEDs counted in on of the mesh routes.
         */
        struct CountedMessage
        {
            uint32_t routeLedCount;
            uint32_t routeNodeCount;
            uint32_t totalLedCount;
            uint32_t totalNodeCount;
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
                CountedMessage countedMessage;
                AudioAnalyzerMessage audioAnalyzerMessage;
            };
        };
    }
}

#endif // CLIENTMESSAGES_H