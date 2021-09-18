#ifndef APIMESSAGES_H
#define APIMESSAGES_H


namespace SyncBlink
{
    namespace Api
    {
        /**
         * @brief   A set of api message types
         */
        enum MessageType
        {
            PING_NODE,
            RENAME_NODE,
            GET_INFO,
            SET_ANALYZER,
            SET_LIGHTMODE,
            LIST_SCRIPTS,
            GET_SCRIPT,
            ADD_SCRIPT,
            UPDATE_SCRIPT,
            DELETE_SCRIPT,
            SET_ACTIVE_SCRIPT,
            UPDATE_FIRMWARE
        };
    }
}

#endif // APIMESSAGES_H