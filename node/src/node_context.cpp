#include "node_context.hpp"

#include <EEPROM.h>
#include <ESP8266httpUpdate.h>

namespace SyncBlink
{
    void NodeContext::setup()
    {     
        _led.setup(readLedCount());
        _led.showNow(Cyan);

        _socketClient
            .meshUpdateEvents
            .addEventHandler([this](Server::UpdateMessage message) { onMeshUpdateReceived(message); });
        _socketClient
            .audioAnalyzerEvents
            .addEventHandler([this](AudioAnalyzerMessage message) { onAnalyzerResultReceived(message); });
        _socketClient
            .meshModEvents
            .addEventHandler([this](std::string mod) { onSocketClientModReceived(mod); });
        _socketClient
            .firmwareFlashEvents
            .addEventHandler([this](std::vector<uint8_t> data, Server::MessageType messageType) { onFirmwareFlashReceived(data, messageType); });
        _socketClient
            .connectionEvents
            .addEventHandler([this](bool connected) { onSocketClientConnectionChanged(connected); });
        _socketServer
            .serverDisconnectionEvents
            .addEventHandler([this](uint64_t clientId) { _socketClient.sendMessage(&clientId, sizeof(clientId), Client::MESH_DISCONNECTION); });
        _socketServer
            .messageEvents
            .addEventHandler([this](Client::MessageType messageType, uint8_t* payload, size_t length) { onSocketServerMessageReceived(messageType, payload, length); });
        
        WiFi.disconnect();
        if(_mesh.tryJoinMesh())
        {
            Serial.printf("Connected to SyncBlink mesh! Starting operation (v%f)...\n", (float)VERSION);
            _socketClient.start(_mesh.getParentIp().toString());
        }
        else
        {
            Serial.println("SyncBlink Station not found - Going to sleep ...");
            _led.showNow(SyncBlink::Black);
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
            _led.showNow(SyncBlink::Black);
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

    void NodeContext::onMeshUpdateReceived(Server::UpdateMessage message)
    {
        _previousLedCount = message.routeLedCount;
        _previousNodeCount = message.routeNodeCount;
        _meshLedCount = message.meshLedCount;
        _meshNodeCount = message.meshNodeCount;   

        if(_socketServer.getClientsCount() != 0)
        {
            message.routeNodeCount++;
            message.routeLedCount += _nodeLedCount;
            _socketServer.broadcast(&message, sizeof(message), Server::MESH_UPDATE);
        }
        else
        {
            _socketClient.sendMessage(0, 0, Client::MESH_UPDATED);
        }
    }

    void NodeContext::onAnalyzerResultReceived(AudioAnalyzerMessage message)
    {
        if(_blinkScript != nullptr)
        {
            uint32_t delta = millis() - _lastUpdate;
            _lastUpdate = millis();
            
            _blinkScript->updateAnalyzerResult(message.volume, message.frequency);
            _blinkScript->run(delta);

            _socketServer.broadcast(&message, sizeof(message), Server::ANALYZER_UPDATE);
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
            _socketClient.sendMessage(0, 0, Client::MOD_DISTRIBUTED);
        }
        else _socketServer.broadcast((void*)mod.c_str(), mod.size(), Server::DISTRIBUTE_MOD);
    }

    void NodeContext::onFirmwareFlashReceived(std::vector<uint8_t> data, Server::MessageType messageType)
    {
        bool restart = false;
        if(messageType == Server::FIRMWARE_FLASH_START)
        {
            _led.blinkNow(Yellow);
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace))
            {
                _led.blinkNow(Red);
                Update.printError(Serial);
            }
            else _led.showNow(Blue);
        }
        else if(messageType == Server::FIRMWARE_FLASH_DATA)
        {            
            if (Update.write(&data[0], data.size()) != data.size())
            {
                _led.blinkNow(Red);
                Update.printError(Serial);
            }
        }
        else if(messageType == Server::FIRMWARE_FLASH_END)
        {
            if (Update.end(true))
            {
                _led.blinkNow(Cyan);
                Serial.println("Update Success: Rebooting...\n");
                restart = true;
            }
            else
            {
                _led.blinkNow(Red);
                Update.printError(Serial);
            }
        }
        _socketServer.broadcast(&data[0], data.size(), messageType);
        // we are restarting here to make sure, that all messages 
        // also get send to the current child nodes
        if(restart)
        {
            _socketServer.loop();
            _led.showNow(Black);
            ESP.restart();
        }
    }

    void NodeContext::onSocketClientConnectionChanged(bool connected)
    {
         if(connected)
         {
            _led.blinkNow(Green);

            Client::ConnectionMessage message = { SyncBlink::getId(), 0, _led.getLedCount(), (float)VERSION };
            _socketClient.sendMessage(&message, sizeof(message), Client::MESH_CONNECTION);
         }
         else _led.blinkNow(Red); 
    }

    void NodeContext::onSocketServerMessageReceived(Client::MessageType messageType, uint8_t* payload, size_t length)
    {
        switch (messageType)
        {
            case Client::MESH_CONNECTION:
            case Client::MESH_DISCONNECTION:
            case Client::MESH_UPDATED:
            case Client::MOD_DISTRIBUTED:
                _socketClient.sendMessage(payload, length, messageType);
                break;
            case Client::EXTERNAL_ANALYZER:
                break;
        }
    }
}