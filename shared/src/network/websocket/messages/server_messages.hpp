#ifndef SERVERMESSAGES_H
#define SERVERMESSAGES_H

#include "audio_analyzer_message.hpp"

#include <array>

namespace SyncBlink
{
    namespace Server
    {
        /**
         * @brief   A set of message types a SyncBlink server will send
         *
         */
        enum MessageType
        {
            MESH_COUNT_REQUEST, // A request to count all LEDs and Nodes in the mesh
            MESH_UPDATE,        // A request with updated LED and node counts
            SOURCE_UPDATE,      // A request with updated analyzer source information
            ANALYZER_UPDATE     // A request with updated analyzer information           
        };

        /**
         * @brief   The count message is used by the *MESH_COUNT_REQUEST*.
         */
        struct CountMessage
        {
            uint32_t treeLedCount;  // The total amount of leds in the current tree
            uint32_t treeNodeCount; // The total amount of nodes in the current tree
        };

        /**
         * @brief   The update message is used by the *MESH_UPDATE*.
         */
        struct UpdateMessage
        {
            uint32_t routeLedCount;
            uint32_t routeNodeCount;
            uint32_t meshLedCount;      // The total amount of LEDs in the mesh
            uint32_t meshNodeCount;     // The total amount of nodes in the mesh
        };

        /**
         * @brief   The *SourceMessage* informs the connected clients
         *          about the currently requested analyzer source.
         *          This will inform the Desktop Application for example that it should
         *          now send Frequency Updates to the Station.
         */
        struct SourceMessage
        {
            AudioAnalyzerSource source;
        };

        /**
         * @brief   The *Message* is a union struct used by the server to
         *          send the different kinds of messages to the clients.
         */
        struct Message
        {
            uint64_t id;
            MessageType messageType;
            union {
                SourceMessage sourceMessage;
                CountMessage countMessage;
                UpdateMessage updateMessage;
                AudioAnalyzerMessage analyzerMessage;
            };
        };
    }
}

#endif // SERVERMESSAGES_H