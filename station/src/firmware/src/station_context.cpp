#include "station_context.hpp"
#include "states/read_mod_state.cpp"
#include "states/fail_safe_state.cpp"
#include "views/splash_view.cpp"

namespace SyncBlink
{
    StationContext::StationContext() : _web(_wifi, _modManager, _nodeManager)
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
            .addEventHandler([this](Client::Message message) { onSocketServerCommandReceived(message); });
        _socketServer
            .serverDisconnectionEvents
            .addEventHandler([this](uint64_t clientId) { onMeshDisconnection(clientId); });
        _wifi.connectWifi();
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

    void StationContext::onSocketServerCommandReceived(Client::Message message)
    {
        switch (message.messageType)
        {
            case Client::MESH_CONNECTION:
            {              
                Serial.printf("Mesh Connection: Client %12llx, LEDs %i, Parent %12llx, Firmware Version: %.2f\n",
                    message.connectionMessage.clientId, message.connectionMessage.ledCount,
                    message.connectionMessage.parentId, message.connectionMessage.firmwareVersion);
                _nodeManager.addNode(message.connectionMessage);

                Server::Message serverMessage = { millis(), Server::MESH_UPDATE };
                Server::UpdateMessage updateMessage = { _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                serverMessage.updateMessage = updateMessage;

                _socketServer.broadcast(serverMessage);
                break;
            }
            case Client::MESH_DISCONNECTION:
                onMeshDisconnection(message.disconnectedClientId);
                break;
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