#include "node_analyzer.hpp"

namespace SyncBlink
{
    NodeAnalyzer::NodeAnalyzer(LED& led, NodeConnector& nodeConnector) : _led(led), _nodeConnector(nodeConnector)
    {
        nodeConnector.getTcpClient()
            .lightModeEvents
            .addEventHandler([this](bool lightMode) { _lightMode = lightMode; _nodeConnector.getTcpServer().broadcast(&lightMode, sizeof(lightMode), Server::LIGHT_MODE); });        
        nodeConnector.getTcpClient()
            .meshScriptEvents
            .addEventHandler([this](std::string script) { onSocketClientScriptReceived(script); });
        nodeConnector.getTcpClient()
            .audioAnalyzerEvents
            .addEventHandler([this](AudioAnalyzerMessage message) { onAnalyzerResultReceived(message); });
        nodeConnector.getTcpClient()
            .sourceUpdateEvents
            .addEventHandler([this](uint64_t nodeId) { _activeAnalyzer = nodeId; _nodeConnector.getTcpServer().broadcast(&nodeId, sizeof(nodeId), Server::SOURCE_UPDATE); });
        nodeConnector.getTcpServer()
            .serverMessageEvents
            .addEventHandler([this](Message message) { onSocketServerMessageReceived(message); });

        _runStandalone = !_nodeConnector.isConnected() && IS_STANDALONE;
    }

    void NodeAnalyzer::loop()
    {
        checkNewScript();

        if(_runStandalone)
        {
            AudioAnalyzerResult result = _frequencyAnalyzer.loop();
            AudioAnalyzerMessage message = result.ToMessage();
            onAnalyzerResultReceived(message);
        }
        else
        {
            checkRunAnalyzer();
        }
    }

    void NodeAnalyzer::toggleLightMode()
    {
        _lightMode = !_lightMode;
    }

    bool NodeAnalyzer::runsStandalone()
    {
        return _runStandalone;
    }

    void NodeAnalyzer::checkNewScript()
    {
        if(_newScript)
        {
            Serial.println("Received new script. Creating new ScriptContext ...");
            _led.clearGroups();
            _blinkScript = std::unique_ptr<SyncBlink::BlinkScript>(new BlinkScript(_led, _currentScript));
            _blinkScript->updateLedInfo(_nodeConnector.getPrevNodeCount(), _nodeConnector.getPrevLedCount(), _nodeConnector.getMeshLedCount());
            _blinkScript->init();
            _newScript = false;
        }
    }

    void NodeAnalyzer::checkRunAnalyzer()
    {
        uint32_t delta = millis() - _lastLedUpdate;
        if(_activeAnalyzer == SyncBlink::getId() && delta > 30) // Just update every 30ms - dont flood mesh
        {
            AudioAnalyzerResult result = _frequencyAnalyzer.loop();
            AudioAnalyzerMessage message = result.ToMessage();

            _nodeConnector.getTcpClient().sendMessage(&message, sizeof(message), Client::EXTERNAL_ANALYZER);
            _lastLedUpdate = millis();
        }
    }

    void NodeAnalyzer::onAnalyzerResultReceived(AudioAnalyzerMessage message)
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
        if(!_runStandalone) _nodeConnector.getTcpServer().broadcast(&message, sizeof(message), Server::ANALYZER_UPDATE);
    }

    void NodeAnalyzer::onSocketClientScriptReceived(std::string script)
    {
        // DON'T create the ScriptContext in the callback
        // Unexpected errors will occure!
        _newScript = true;
        _currentScript = script;
        _nodeConnector.getTcpClient().sendMessage(0, 0, Client::SCRIPT_DISTRIBUTED);
        _nodeConnector.getTcpServer().broadcast((void*)script.c_str(), script.size(), Server::DISTRIBUTE_SCRIPT);
    }    

    void NodeAnalyzer::onSocketServerMessageReceived(Message message)
    {
        switch (message.type)
        {
            case Client::MESH_CONNECTION:
            case Client::MESH_DISCONNECTION:
            case Client::MESH_UPDATED:
            case Client::SCRIPT_DISTRIBUTED:
                _nodeConnector.getTcpClient().sendMessage(&message.body[0], message.body.size(), (Client::MessageType)message.type);
                break;            
            case Client::EXTERNAL_ANALYZER:
                AudioAnalyzerMessage analyzerMessage;
                memcpy(&analyzerMessage, &message.body[0], message.body.size());
                onAnalyzerResultReceived(analyzerMessage);

                _nodeConnector.getTcpClient().sendMessage(&message.body[0], message.body.size(), (Client::MessageType)message.type);
                break;
        }
    }
}