#include "script_broadcast_module.hpp"

#include <LittleFS.h>

namespace SyncBlink
{
    ScriptBroadcastModule::ScriptBroadcastModule(MessageBus& messageBus, MeshInfo& meshInfo, ScriptList& scriptList) : _messageBus(messageBus), _meshInfo(meshInfo), _scriptList(scriptList)
    {
        _meshHandleId = _messageBus.addMsgHandler<Messages::MeshConnection>(MessageType::MeshConnection, this);
        _scriptHandleId = _messageBus.addMsgHandler<Messages::ScriptLoad>(MessageType::ScriptLoad, this);
    }

    ScriptBroadcastModule::~ScriptBroadcastModule()
    {
        _messageBus.removeMsgHandler(_meshHandleId);
        _messageBus.removeMsgHandler(_scriptHandleId);
    }

    void ScriptBroadcastModule::loop()
    {
        auto iter = _broadcastSessions.begin();
        auto endIter = _broadcastSessions.end();
        for (; iter != endIter;)
        {
            if(advanceSession(iter))
            {
                endSession(iter, false);
                iter = _broadcastSessions.erase(iter);
            }
            else ++iter;
        }
    }

    void ScriptBroadcastModule::onMsg(const Messages::MeshConnection& msg)
    {
        if(!msg.isConnected) return;
        addSession(msg.nodeId, _meshInfo.getActiveScript().Name);
    }

    void ScriptBroadcastModule::onMsg(const Messages::ScriptLoad& msg)
    {
        addSession(0, msg.scriptName);
    }
        
    void ScriptBroadcastModule::addSession(uint64_t recipientId, std::string scriptName)
    {
        Script script = _scriptList.get(scriptName);
        if(recipientId == 0)
        {
            // ScriptLoad on Station was executed, end all active sessions to broadcast the new script to all nodes
            for (auto iter = _broadcastSessions.begin(); iter != _broadcastSessions.end(); iter++)
                endSession(iter, false);
            _broadcastSessions.clear();
        }
        else
        {
            auto activeSession = _broadcastSessions.find(recipientId);
            if(activeSession != _broadcastSessions.end()) endSession(activeSession, true);
        }
        startSession(recipientId, scriptName);
        _broadcastSessions[recipientId] = _scriptList.get(scriptName);
    }

    void ScriptBroadcastModule::startSession(uint64_t recipientId, std::string scriptName)
    {                
        Messages::NodeCommand msg;
        msg.recipientId = recipientId;
        msg.commandInfo.stringInfo1 = scriptName;
        msg.commandType = Messages::NodeCommandType::ScriptUpdate;

        _messageBus.trigger(msg);
    }

    bool ScriptBroadcastModule::advanceSession(std::map<uint64_t, SyncBlink::Script>::iterator activeSession)
    {
        bool finished = false;

        Messages::RawBytes msg;
        msg.recipientId = activeSession->first;
        msg.byteCount = 0;

        File& bytecodeFile = activeSession->second.getBytecodeFile();
        while(bytecodeFile.available() && msg.byteCount < 100)
        {
            msg.byteCount++;
            msg.bytes.push_back(bytecodeFile.read());
        }
        if(!bytecodeFile.available()) finished = true;

        _messageBus.trigger(msg);
        return finished;
    }

    void ScriptBroadcastModule::endSession(std::map<uint64_t, SyncBlink::Script>::iterator activeSession, bool removeFromMap)
    {
        activeSession->second.closeFile();
                
        Messages::NodeCommand msg;
        msg.recipientId = activeSession->first;
        msg.commandType = Messages::NodeCommandType::ScriptUpdated;

        _messageBus.trigger(msg);
        if(removeFromMap) _broadcastSessions.erase(activeSession);
    }
}