#include "node_context.hpp"

#include <EEPROM.h>
#include <ESP8266httpUpdate.h>
#include <shared_constants.hpp>

namespace SyncBlink
{
    void NodeContext::setup()
    {     
        readNodeInfo();
        _led.setup(_nodeLedCount);
        _led.showNow(Cyan);

        _tcpClient
            .pingEvents
            .addEventHandler([this](uint64_t nodeId) { if(nodeId == SyncBlink::getId()) _led.blinkNow(Yellow, 5); else _tcpServer.broadcast(&nodeId, sizeof(nodeId), Server::PING);});
        _tcpClient
            .lightModeEvents
            .addEventHandler([this](bool lightMode) { _lightMode = lightMode; _tcpServer.broadcast(&lightMode, sizeof(lightMode), Server::LIGHT_MODE); });
        _tcpClient
            .connectionEvents
            .addEventHandler([this](bool connected) { onSocketClientConnectionChanged(connected); });
        _tcpClient
            .meshUpdateEvents
            .addEventHandler([this](Server::UpdateMessage message) { onMeshUpdateReceived(message); });
        _tcpClient
            .meshScriptEvents
            .addEventHandler([this](std::string script) { onSocketClientScriptReceived(script); });
        _tcpClient
            .audioAnalyzerEvents
            .addEventHandler([this](AudioAnalyzerMessage message) { onAnalyzerResultReceived(message); });
        _tcpClient
            .nodeRenameEvents
            .addEventHandler([this](Server::NodeRenameMessage message) { onNodeRenameReceived(message); });
        _tcpClient
            .firmwareFlashEvents
            .addEventHandler([this](std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType) { onFirmwareFlashReceived(data, targetNodeId, messageType); });
        _tcpClient
            .sourceUpdateEvents
            .addEventHandler([this](uint64_t nodeId) { _activeAnalyzer = nodeId; _tcpServer.broadcast(&nodeId, sizeof(nodeId), Server::SOURCE_UPDATE); });

        _tcpServer
            .serverDisconnectionEvents
            .addEventHandler([this](uint64_t nodeId) { _tcpClient.sendMessage(&nodeId, sizeof(nodeId), Client::MESH_DISCONNECTION); });
        _tcpServer
            .messageEvents
            .addEventHandler([this](TcpMessage message) { onSocketServerMessageReceived(message); });
        
        WiFi.disconnect();
        if(_mesh.tryJoinMesh())
        {
            Serial.printf("Connected to SyncBlink mesh! Starting operation (v%i.%i)...\n", VERSIONMAJOR, VERSIONMINOR);
            _tcpClient.start(_mesh.getParentIp().toString());
            _tcpServer.start();
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
        checkNewScript();
        
        #ifdef MODE_PIN
        checkModeButton();
        #endif

        #ifdef IS_ANALYZER
        runAnalyzer();
        #endif

        _tcpClient.loop();
        _tcpServer.loop();
        _led.loop();

        if((!_mesh.isConnected() || _tcpClient.isDiscontinued()))
        {
            Serial.printf("Wifi: %i - Tcp: %i - Going to sleep ...\n", _mesh.isConnected(), _tcpClient.isDiscontinued());
            _led.blinkNow(Red);
            _led.showNow(SyncBlink::Black);
            ESP.deepSleep(SleepSeconds * 1000000);
        }
    }

    void NodeContext::readNodeInfo()
    {
        _nodeLedCount = 0;
        for(int i = 0; i < 4; i++)
        {
            _nodeLedCount += EEPROM.read(i) << (i*8);
        }
        Serial.printf("EEPROM LED Count: %i\n", _nodeLedCount);
        if(EEPROM.read(4) != '\0')
        {
            char label[MaxNodeLabelLength];
            for(uint8_t i = 0; i < MaxNodeLabelLength; i++)
            {
                label[i] = EEPROM.read(i+4);
            }
            _nodeLabel = std::string(label);
            Serial.printf("EEPROM Node Label: %s\n", _nodeLabel.c_str());
        }
        else Serial.println("EEPROM Node Label: (No Label)");
    }

    void NodeContext::checkNewScript()
    {
        if(_newScript)
        {
            Serial.println("Received new script. Creating new ScriptContext ...");
            _led.clearGroups();
            _blinkScript = std::unique_ptr<SyncBlink::BlinkScript>(new BlinkScript(_led, _currentScript));
            _blinkScript->updateLedInfo(_previousNodeCount, _previousLedCount, _meshLedCount);
            _blinkScript->init();
            _newScript = false;
        }
    }

    #ifdef MODE_PIN
    void NodeContext::checkModeButton()
    {
        if(_lastButtonUpdate + 100 > millis()) return;

        char buttonVal = digitalRead(MODE_PIN);
        if(buttonVal == HIGH && _lastButtonVal == LOW)
        {
            _lightMode = !_lightMode;
        }
        _lastButtonVal = buttonVal;
        _lastButtonUpdate = millis();
    }
    #endif

    #ifdef IS_ANALYZER
    void NodeContext::runAnalyzer()
    {
        uint32_t delta = millis() - _lastLedUpdate;
        if(_activeAnalyzer == SyncBlink::getId() && delta > 30) // Just update every 30ms - dont flood mesh
        {
            AudioAnalyzerResult result = _frequencyAnalyzer.loop();
            AudioAnalyzerMessage message = result.ToMessage();

            _tcpClient.sendMessage(&message, sizeof(message), Client::EXTERNAL_ANALYZER);
            _lastLedUpdate = millis();
        }
    }
    #endif

    void NodeContext::onMeshUpdateReceived(Server::UpdateMessage message)
    {
        _previousLedCount = message.routeLedCount;
        _previousNodeCount = message.routeNodeCount;
        _meshLedCount = message.meshLedCount;
        _meshNodeCount = message.meshNodeCount;   

        if(_tcpServer.getClientsCount() != 0)
        {
            message.routeNodeCount++;
            message.routeLedCount += _nodeLedCount;
            _tcpServer.broadcast(&message, sizeof(message), Server::MESH_UPDATE);
        }
        else
        {
            _tcpClient.sendMessage(0, 0, Client::MESH_UPDATED);
        }
    }

    void NodeContext::onAnalyzerResultReceived(AudioAnalyzerMessage message)
    {
        if(_blinkScript != nullptr && !_lightMode)
        {
            uint32_t delta = millis() - _lastUpdate;
            _lastUpdate = millis();
            
            _blinkScript->updateAnalyzerResult(message.volume, message.frequency);
            _blinkScript->run(delta);
        }
        else if(_lightMode)
        {
            _led.showNow(SyncBlink::White);
        }

        // Always forward the update, because there could be nodes which switched the light mode off via a mode button!
        _tcpServer.broadcast(&message, sizeof(message), Server::ANALYZER_UPDATE);
    }

    void NodeContext::onSocketClientScriptReceived(std::string script)
    {
        // DON'T create the ScriptContext in the callback
        // Unexpected errors will occure!
        _newScript = true;
        _currentScript = script;
        _tcpClient.sendMessage(0, 0, Client::SCRIPT_DISTRIBUTED);
        _tcpServer.broadcast((void*)script.c_str(), script.size(), Server::DISTRIBUTE_SCRIPT);
    }

    void NodeContext::onNodeRenameReceived(Server::NodeRenameMessage message)
    {
        if(message.nodeId == SyncBlink::getId())
        {
            for(uint8_t i = 0; i < MaxNodeLabelLength; i++)
            {
                EEPROM.write(i+4, message.nodeLabel[i]);
            }
            EEPROM.commit();
            readNodeInfo();
        }
        else _tcpServer.broadcast(&message, sizeof(message), Server::NODE_RENAME);
    }

    void NodeContext::onFirmwareFlashReceived(std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType)
    {
        bool restart = false;
        if(targetNodeId == SyncBlink::getId() || targetNodeId == 0)
        {
            if(messageType == Server::FIRMWARE_FLASH_START)
            {
                _led.blinkNow(Yellow);
                uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                if (!Update.begin(maxSketchSpace))
                {
                    _led.blinkNow(Red);
                    Update.printError(Serial);
                }
                else
                {
                    _led.showNow(Blue);
                    _flashActive = true;
                }
            }
            else if(messageType == Server::FIRMWARE_FLASH_END)
            {
                if (Update.end(true))
                {
                    _led.blinkNow(Cyan);                    
                    _flashActive = false;
                    Serial.println("Update Success: Rebooting...\n");
                    restart = true;
                }
                else
                {
                    _led.blinkNow(Red);
                    Update.printError(Serial);
                }
            }
        }   
        
        // NodeId for FLash Data is '0' - that why we handle it in a seperate if branch
        if(_flashActive && messageType == Server::FIRMWARE_FLASH_DATA)
        {            
            if (Update.write(&data[0], data.size()) != data.size())
            {
                _led.blinkNow(Red);
                Update.printError(Serial);
            }
        }
        
        // Only distribute, if the message is not for the current node
        // Or all nodes in the mesh get flashed
        if(!_flashActive ||  targetNodeId == 0) _tcpServer.broadcast(&data[0], data.size(), messageType);

        // we are restarting here to make sure, that all messages 
        // also get send to the current child nodes
        if(restart)
        {
            _tcpServer.loop();
            _led.showNow(Black);
            ESP.restart();
        }
    }

    void NodeContext::onSocketClientConnectionChanged(bool connected)
    {
         if(connected)
         {
            _led.blinkNow(Green);

            Client::ConnectionMessage message = { false, _isAnalyzer, true, SyncBlink::getId(), 0, _led.getLedCount(), VERSIONMAJOR, VERSIONMINOR };
            memcpy(&message.nodeLabel[0], &_nodeLabel[0], _nodeLabel.size());

            _tcpClient.sendMessage(&message, sizeof(message), Client::MESH_CONNECTION);
         }
         else _led.blinkNow(Red); 
    }

    void NodeContext::onSocketServerMessageReceived(TcpMessage message)
    {
        switch (message.messageType)
        {
            case Client::MESH_CONNECTION:
            case Client::MESH_DISCONNECTION:
            case Client::MESH_UPDATED:
            case Client::SCRIPT_DISTRIBUTED:
                _tcpClient.sendMessage(&message.message[0], message.message.size(), (Client::MessageType)message.messageType);
                break;            
            case Client::EXTERNAL_ANALYZER:
                AudioAnalyzerMessage analyzerMessage;
                memcpy(&analyzerMessage, &message.message[0], message.message.size());
                onAnalyzerResultReceived(analyzerMessage);

                _tcpClient.sendMessage(&message.message[0], message.message.size(), (Client::MessageType)message.messageType);
                break;
        }
    }
}