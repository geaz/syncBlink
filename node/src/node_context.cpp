#include "node_context.hpp"

#include <EEPROM.h>
#include <ESP8266httpUpdate.h>

namespace SyncBlink
{
    void NodeContext::setup()
    {     
        _led.setup(readLedCount());
        _socketServer
            .messageEvents
            .addEventHandler([this](Client::Message message) { onSocketServerMessageReceived(message); });
        _socketClient
            .messageEvents
            .addEventHandler([this](Server::Message message) { onSocketClientMessageReceived(message); });
        _socketClient
            .meshModEvents
            .addEventHandler([this](std::string mod) {onSocketClientModReceived(mod); });
        _socketClient
            .connectionEvents
            .addEventHandler([this](bool isConnected) { if(isConnected) _led.blinkNow(Green); else _led.blinkNow(Red); });
        
        WiFi.disconnect();
        if(_mesh.tryJoinMesh())
        {
            Serial.printf("Connected to SyncBlink mesh! Starting operation (v%f)...\n", (float)VERSION);
            _socketClient.start(_mesh.getParentIp().toString(), (float)VERSION);
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

    uint32_t NodeContext::readLedCount()
    {
        uint32_t leds = 0;
        for(int i = 0; i < 4; i++)
        {
            leds += EEPROM.read(i) << (i*8);
        }
        Serial.printf("EEPROM LED Count: %i\n", leds);
        return leds;
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

    void NodeContext::onSocketClientMessageReceived(Server::Message message)
    {
        switch (message.messageType)
        {
            case Server::START_OTA_UPDATE:
            {
                _led.setAllLeds(Yellow);
                _led.loop();
                ESPhttpUpdate.update(_mesh.getParentIp().toString(), 80, "/ota");
                break;
            }
            case Server::MESH_COUNT_REQUEST:
            {
                if(_socketServer.getClientsCount() != 0)
                {
                    // Start new tree Count
                    message.countMessage.treeLedCount = 0;
                    message.countMessage.treeNodeCount = 0;
                    _socketServer.broadcast(message);
                }
                else
                {
                    Client::Message answerMessage = { message.id, Client::MESH_COUNTED };
                    answerMessage.countedMessage = { _nodeLedCount, 1, _nodeLedCount, 1 };
                    _socketClient.sendMessage(answerMessage);
                }
                break;
            }
            case Server::MESH_UPDATE:
            {
                _previousLedCount = message.updateMessage.routeLedCount;
                _previousNodeCount = message.updateMessage.routeNodeCount;
                _meshLedCount = message.updateMessage.meshLedCount;
                _meshNodeCount = message.updateMessage.meshNodeCount;   

                if(_socketServer.getClientsCount() != 0)
                {
                    message.updateMessage.routeNodeCount++;
                    message.updateMessage.routeLedCount += _nodeLedCount;
                    _socketServer.broadcast(message);
                }
                else
                {
                    Client::Message answerMessage = { message.id, Client::MESH_UPDATED };
                    _socketClient.sendMessage(answerMessage);
                }
                break;
            }
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
            Client::Message message = { 0, Client::MOD_DISTRIBUTED };
            _socketClient.sendMessage(message);
        }
        else _socketServer.broadcastMod(mod);
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