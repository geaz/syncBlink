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

    void StationContext::startMeshCount()
    {
        Serial.println("Connection state changed. Starting MESH_COUNT_REQUEST ...");
        if(_socketServer.getClientsCount() == 0)
        {
            Serial.println("No nodes connected!");
            Serial.printf("SyncBlink Station alone with %d LEDs :(\n", _led.getLedCount());
            _nodeManager.counted = { _led.getLedCount(), 1, _led.getLedCount(), 1 };
        }
        else
        {
            Server::CountMessage countMessage = { 0, 0 };
            Server::Message message = { millis(), Server::MESH_COUNT_REQUEST };
            message.countMessage = countMessage;
            
            _socketServer.broadcast(message);
        }
    }

    void StationContext::onSocketServerCommandReceived(Client::Message message)
    {
        switch (message.messageType)
        {
            case Client::MESH_CONNECTION:
                startMeshCount();
                break;
            case Client::MESH_COUNTED:
            {
                _nodeManager.counted = message.countedMessage;
                Serial.printf("MESH_COUNT done with %d Nodes total.\n", _nodeManager.counted.totalNodeCount);
                Serial.println("Distributing result of count ...");

                Server::Message message = { millis(), Server::MESH_UPDATE };
                Server::UpdateMessage updateMessage = { _led.getLedCount(), 1, _nodeManager.counted.totalLedCount, _nodeManager.counted.totalNodeCount };
                message.updateMessage = updateMessage;

                _socketServer.broadcast(message);
                break;
            }
            case Client::MESH_UPDATED:
                Serial.println("Mesh updated - Resetting state ...");
                currentState = std::make_shared<ReadModState>();
                break;
            case Client::NONE:
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