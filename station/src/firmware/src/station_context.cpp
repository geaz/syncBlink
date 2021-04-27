#include "station_context.hpp"
#include "states/read_mod_state.cpp"
#include "states/fail_safe_state.cpp"
#include "views/splash_view.cpp"

namespace SyncBlink
{
    StationContext::StationContext() : _nodeManager(_socketServer), _web(_wifi, _modManager, _nodeManager)
    {
        resetState();
        checkReset();
    }

    void StationContext::setup()
    {        
        _display.init();
        _display.setView(std::make_shared<SyncBlink::SplashView>());
        _display.loop();
        _led.setup(LED_COUNT);

        _socketServer
            .messageEvents
            .addEventHandler([this](Client::MessageType messageType, uint8_t* payload, size_t length) { onSocketServerCommandReceived(messageType, payload, length); });
        _socketServer
            .serverDisconnectionEvents
            .addEventHandler([this](uint64_t clientId) { onMeshDisconnection(clientId); });

        _wifi.connectWifi();
        _nodeManager.addNode({ SyncBlink::getId(), 0, LED_COUNT, 0 });
    }

    void StationContext::loop()
    {
        _display.setLeftStatus("");
        _display.setRightStatus(WiFi.localIP().toString().c_str());
        currentState->run(*this);
        _socketServer.loop();
        _led.loop();
        _web.loop();
        _display.loop();
        _nodeManager.loopFlashing();
    }

    void StationContext::resetState() 
    { 
        currentState = std::make_shared<ReadModState>(); 
    }

    void StationContext::checkReset()
    {
        auto rstPtr = ESP.getResetInfoPtr();
        if(rstPtr->reason >= 1 && rstPtr->reason <= 4) currentState = std::make_shared<FailSafeState>(*this); 
    }

    void StationContext::onMeshDisconnection(uint64_t clientId)
    {
        Serial.printf("Mesh Disconnection: %12llx\n", clientId);
        _nodeManager.removeNode(clientId);
    }

    void StationContext::onSocketServerCommandReceived(Client::MessageType messageType, uint8_t* payload, size_t length)
    {
        switch (messageType)
        {
            case Client::MESH_CONNECTION:
            {              
                Client::ConnectionMessage message;
                memcpy(&message, payload, length);

                Serial.printf("Mesh Connection: Client %12llx, LEDs %i, Parent %12llx, Firmware Version: %.2f\n",
                    message.clientId, message.ledCount,
                    message.parentId, message.firmwareVersion);
                _nodeManager.addNode(message);

                Server::UpdateMessage updateMessage = { _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                _socketServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
                break;
            }
            case Client::MESH_DISCONNECTION:
            {
                uint64_t clientId;
                memcpy(&clientId, payload, length);
                onMeshDisconnection(clientId);
                break;
            }
            case Client::MESH_UPDATED:
                Serial.println("Mesh updated - Resetting state ...");
                currentState = std::make_shared<ReadModState>();
                break;
            case Client::EXTERNAL_ANALYZER:
            case Client::MOD_DISTRIBUTED:
                break;
        }
    }

    LED& StationContext::getLed() { return _led; }
    Display& StationContext::getDisplay() { return _display; }
    ModManager& StationContext::getModManager() { return _modManager; }
    StationWifi& StationContext::getStationWifi() { return _wifi; }
    SocketServer& StationContext::getSocketServer() { return _socketServer; }
    NodeManager& StationContext::getNodeManager() { return _nodeManager; }
}