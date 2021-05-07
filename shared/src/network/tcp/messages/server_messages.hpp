#ifndef SERVERMESSAGES_H
#define SERVERMESSAGES_H

#include "shared_constants.hpp"
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
            PING,                   // A request to ping all nodes - they dont have to react
            MESH_UPDATE,            // A request with updated LED and node counts
            SOURCE_UPDATE,          // A request with updated analyzer source information
            ANALYZER_UPDATE,        // A request with updated analyzer information
            DISTRIBUTE_SCRIPT,      // A request to distribute a led script
            NODE_RENAME,            // A request to rename a node
            FIRMWARE_FLASH_START,   // A request to flash a new node firmware
            FIRMWARE_FLASH_DATA,    // A request with firmware data
            FIRMWARE_FLASH_END      // A request to send the end of a firmware transmission
        };

        /**
         * @brief   The update message is used by the *MESH_UPDATE*.
         */
        struct UpdateMessage
        {
            uint64_t activeSource;
            uint32_t routeLedCount;
            uint32_t routeNodeCount;
            uint32_t meshLedCount;      // The total amount of LEDs in the mesh
            uint32_t meshNodeCount;     // The total amount of nodes in the mesh
        };

        struct NodeRenameMessage
        {
            uint64_t nodeId;
            char nodeLabel[MaxNodeLabelLength];
        };
    }
}

#endif // SERVERMESSAGES_H