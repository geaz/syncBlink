#include "station_context.hpp"
#include "states/read_mod_state.cpp"
#include "states/fail_safe_state.cpp"
#include "states/receiving_firmware_state.cpp"
#include "views/splash_view.cpp"

namespace SyncBlink
{
    StationContext::StationContext() : _nodeManager(_tcpServer), _web(_wifi, _modManager, _nodeManager)
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
            .addEventHandler([this](uint64_t clientId) { onMeshDisconnection(clientId); });

        _web.uploadListener
            .addEventHandler([this](float progress, bool isStart, bool isEnd, bool isError) {
                if(isStart) currentState = std::make_shared<ReceivingFirmwareState>(*this);
                currentState->run(*this);
            });

        _wifi.connectWifi();
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
        _display.setRightStatus(WiFi.localIP().toString().c_str());

        currentState->run(*this);
        
        _tcpServer.loop();
        _led.loop();
        _web.loop();
        _display.loop();
    }
    
    void StationContext::resetState() 
    { 
        currentState = std::make_shared<ReadModState>(); 
    }

    void StationContext::checkException()
    {
        auto rstPtr = ESP.getResetInfoPtr();
        if(rstPtr->reason >= 1 && rstPtr->reason <= 4) currentState = std::make_shared<FailSafeState>(*this); 
    }

    void StationContext::onMeshDisconnection(uint64_t clientId)
    {
        _nodeManager.removeNode(clientId);
    }

    void StationContext::onSocketServerCommandReceived(TcpMessage tcpMessage)
    {
        switch (tcpMessage.messageType)
        {
            case Client::MESH_CONNECTION:
            {              
                Client::ConnectionMessage message;
                memcpy(&message, &tcpMessage.message[0], tcpMessage.message.size());
                
                _nodeManager.addNode(message);

                Server::UpdateMessage updateMessage = { _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
                break;
            }
            case Client::MESH_DISCONNECTION:
            {
                uint64_t clientId;
                memcpy(&clientId, &tcpMessage.message[0], tcpMessage.message.size());
                onMeshDisconnection(clientId);

                Server::UpdateMessage updateMessage = { _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
                break;
            }
            case Client::MESH_UPDATED:
            case Client::EXTERNAL_ANALYZER:
            case Client::MOD_DISTRIBUTED:
                break;
        }
    }

    LED& StationContext::getLed() { return _led; }
    Display& StationContext::getDisplay() { return _display; }
    ModManager& StationContext::getModManager() { return _modManager; }
    SyncBlinkWeb& StationContext::getWebserver() { return _web; }
    TcpServer& StationContext::getTcpServer() { return _tcpServer; }
    NodeManager& StationContext::getNodeManager() { return _nodeManager; }
}