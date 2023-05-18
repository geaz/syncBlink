#ifndef SCRIPTBROADCASTMODULE_H
#define SCRIPTBROADCASTMODULE_H

#include "core/message/message_bus.hpp"
#include "core/message/messages/mesh_connection.hpp"
#include "core/message/messages/script_load.hpp"
#include "core/mesh_info.hpp"
#include "core/script_list.hpp"
#include "core/script.hpp"
#include "module.hpp"

#include <map>
#include <string>

namespace SyncBlink
{
    class ScriptBroadcastModule : public Module,
                         public MessageHandler<Messages::MeshConnection>,
                         public MessageHandler<Messages::ScriptLoad>
    {
    public:
        ScriptBroadcastModule(MessageBus& messageBus, MeshInfo& meshInfo, ScriptList& scriptList);
        ~ScriptBroadcastModule();

        void loop() override;

        void onMsg(const Messages::MeshConnection& msg);
        void onMsg(const Messages::ScriptLoad& msg);

    private:
        void addSession(uint64_t recipientId, std::string scriptName);
        void startSession(uint64_t recipientId, std::string scriptName);
        bool advanceSession(std::map<uint64_t, SyncBlink::Script>::iterator activeSession);
        void endSession(std::map<uint64_t, SyncBlink::Script>::iterator activeSession, bool removeFromMap);

        MessageBus& _messageBus;
        MeshInfo& _meshInfo;
        ScriptList& _scriptList;

        uint32_t _meshHandleId = 0;
        uint32_t _scriptHandleId = 0;

        std::map<uint64_t, Script> _broadcastSessions;
    };
}

#endif // SCRIPTBROADCASTMODULE_H