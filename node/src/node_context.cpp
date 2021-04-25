#include "node_context.hpp"

namespace SyncBlink
{
    void NodeContext::setup()
    {
        _socketServer
            .meshConnectionEvents
            .addEventHandler([this]() { onSocketServerMeshConnection(); });
        _socketServer
            .messageEvents
            .addEventHandler([this](Client::Message message) { onSocketServerMessageReceived(message); });
        _socketClient
            .messageEvents
            .addEventHandler([this](Server::Message message) { onSocketClientMessageReceived(message); });
        _socketClient
            .meshModEvents
            .addEventHandler([this](std::string mod) {onSocketClientModReceived(mod); });
        
        WiFi.disconnect();
        if(_mesh.tryJoinMesh())
        {
            Serial.println("Connected to SyncBlink mesh! Starting operation ...");
            _socketClient.start(_mesh.getParentIp().toString());
        }
        else
        {
            Serial.println("SyncBlink Station not found - Going to sleep ...");
            ESP.deepSleep(SleepSeconds * 1000000);
        }
    }

    void NodeContext::loop()
    {
        checkNewMod();
        
        _socketClient.loop();
        _socketServer.loop();
        _led.loop();
        if(!_mesh.isConnected())
        {
            Serial.println("Websocket and WiFi disconnected! Going to sleep ...");
            _led.setAllLeds(SyncBlink::Black);
            _led.loop();
            ESP.deepSleep(SleepSeconds * 1000000);
        }
    }

    void NodeContext::checkNewMod()
    {
        if(_newMod)
        {
            Serial.println("Received new mod. Creating new ScriptContext ...");
            _led.clearGroups();
            _blinkScript = std::unique_ptr<SyncBlink::BlinkScript>(new BlinkScript(_led, _currentMod));
            _blinkScript->updateLedInfo(_previousNodeCount, _previousLedCount, _meshLedCount);
            _blinkScript->init();
            _newMod = false;
        }
    }

    void NodeContext::checkDistributeOrAnswer(Server::Message message, Client::MessageType answerMessageType)
    {
        if(_socketServer.getClientsCount() == 0)
        {
            Client::Message answerMessage = { message.id, answerMessageType };
            if(answerMessageType == Client::MESH_COUNTED)
            {
                answerMessage.countedMessage = { 
                    message.countMessage.routeLedCount, message.countMessage.routeNodeCount, 
                    message.countMessage.routeLedCount, message.countMessage.routeNodeCount 
                };
            }
            _socketClient.sendMessage(answerMessage);
        }
        else { _socketServer.broadcast(message); }
    }

    void NodeContext::onSocketClientMessageReceived(Server::Message message)
    {
        switch (message.messageType)
        {
            case Server::MESH_COUNT_REQUEST:
                Serial.println("MESH_COUNT_REQUEST received...");
                
                _previousLedCount = message.countMessage.routeLedCount;
                _previousNodeCount = message.countMessage.routeNodeCount;

                message.countMessage.routeNodeCount++;
                message.countMessage.routeLedCount += LED_COUNT;

                checkDistributeOrAnswer(message, Client::MESH_COUNTED);
                break;
            case Server::MESH_UPDATE:
                _meshLedCount = message.updateMessage.meshLedCount;
                _meshNodeCount = message.updateMessage.meshNodeCount;                
                checkDistributeOrAnswer(message, Client::MESH_UPDATED);
                break;
            case Server::ANALYZER_UPDATE:
                if(_blinkScript != nullptr)
                {
                    uint32_t delta = millis() - _lastUpdate;
                    _lastUpdate = millis();
                    
                    _blinkScript->updateAnalyzerResult(message.analyzerMessage.volume, message.analyzerMessage.frequency);
                    _blinkScript->run(delta);                    
                    _socketServer.broadcast(message);
                }
                break;
            case Server::SOURCE_UPDATE:
                break;
        }
    }

    void NodeContext::onSocketClientModReceived(std::string mod)
    {
        // DON'T create the ScriptContext in the callback
        // Unexpected errors will occure!
        _newMod = true;
        _currentMod = mod;
        if(_socketServer.getClientsCount() == 0)
        {
            Serial.println("End of route - Sending MOD_DISTRIBUTED");
            Client::Message message = { millis(), Client::MOD_DISTRIBUTED };
            _socketClient.sendMessage(message);
        }
        else _socketServer.broadcastMod(mod);
    }    

    void NodeContext::onSocketServerMeshConnection()
    {
        Serial.println("Got a new connection. Informing parent node ...");
        Client::Message message = { millis(), Client::MESH_CONNECTION };
        _socketClient.sendMessage(message);
    }

    void NodeContext::onSocketServerMessageReceived(Client::Message message)
    {
        switch (message.messageType)
        {
            case Client::MESH_CONNECTION:
            case Client::MESH_COUNTED:
            case Client::MESH_UPDATED:
            case Client::MOD_DISTRIBUTED:
                _socketClient.sendMessage(message);
                break;
            case Client::NONE:
            case Client::EXTERNAL_ANALYZER:
                break;
        }
    }
}