#include "station_context.hpp"
#include "states/fail_safe_state.cpp"
#include "states/broadcast_script_state.cpp"
#include "states/receiving_firmware_state.cpp"
#include "views/splash_view.cpp"

namespace SyncBlink
{
    StationContext::StationContext() : _nodeManager(_tcpServer)
    {
        resetState();
        checkException();
    }

    void StationContext::setup()
    {
        _display.init();
        _display.setView(std::make_shared<SyncBlink::SplashView>());
        _display.loop();
        _led.setup(LED_COUNT);

        _tcpServer.messageEvents
            .addEventHandler([this](TcpMessage message) {
                onSocketServerCommandReceived(message);
            });

        _tcpServer.serverDisconnectionEvents
            .addEventHandler([this](uint64_t nodeId) { onMeshDisconnection(nodeId); });

        _tcpServer.start();
    }

    void StationContext::loop()
    {
        #ifdef LOG_HEAP
        if(millis() - _lastHeapLog > 1000)
        {
            Serial.println(ESP.getFreeHeap());
            _lastHeapLog = millis();
        }
        #endif
        _display.setLeftStatus("");

        currentState->run(*this);
        
        _tcpServer.loop();
        _led.loop();
        _display.loop();
    }
    
    void StationContext::resetState() 
    { 
        currentState = std::make_shared<BroadcastScriptState>(*this); 
    }

    void StationContext::checkException()
    {
        auto rstPtr = ESP.getResetInfoPtr();
        if(rstPtr->reason >= 1 && rstPtr->reason <= 4) currentState = std::make_shared<FailSafeState>(*this); 
    }

    void StationContext::onMeshDisconnection(uint64_t nodeId)
    {
        _nodeManager.removeNode(nodeId);
    }

    void StationContext::onSocketServerCommandReceived(TcpMessage tcpMessage)
    {
        switch (tcpMessage.messageType)
        {
            case Client::MESH_CONNECTION:
            {              
                Client::ConnectionMessage message;
                memcpy(&message, &tcpMessage.message[0], tcpMessage.message.size());
                
                if(message.isAnalyzer && !message.isNode)
                {
                    Serial.printf("[MESH] New Analyzer connected: %s\n", message.nodeLabel);
                }
                else
                {
                    Serial.printf("[MESH] New Node: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                        message.nodeId, message.ledCount,
                        message.parentId, message.majorVersion, message.minorVersion);
                }

                _nodeManager.addNode(message);

                Server::UpdateMessage updateMessage = { _nodeManager.getActiveAnalyzer(), _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
                break;
            }
            case Client::MESH_DISCONNECTION:
            {
                uint64_t nodeId;
                memcpy(&nodeId, &tcpMessage.message[0], tcpMessage.message.size());
                onMeshDisconnection(nodeId);

                Serial.printf("[MESH] Node disconnected: %12llx\n", nodeId);

                Server::UpdateMessage updateMessage = { _nodeManager.getActiveAnalyzer(), _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
                break;
            }
            // Handled in different station states
            case Client::MESH_UPDATED:
            case Client::EXTERNAL_ANALYZER:
            case Client::SCRIPT_DISTRIBUTED:
                break;
        }
    }

    LED& StationContext::getLed() { return _led; }
    Display& StationContext::getDisplay() { return _display; }
    ScriptManager& StationContext::getScriptManager() { return _ScriptManager; }
    TcpServer& StationContext::getTcpServer() { return _tcpServer; }
    NodeManager& StationContext::getNodeManager() { return _nodeManager; }
    uint64_t StationContext::getStationId() const { return _stationId; }
}