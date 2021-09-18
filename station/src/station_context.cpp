#include "station_context.hpp"
#include "states/fail_safe_state.cpp"
#include "states/broadcast_script_state.cpp"
#include "states/receiving_firmware_state.cpp"
#include "display/views/splash_view.cpp"

namespace SyncBlink
{
    StationContext::StationContext() : _nodeManager(_led, _tcpServer)
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
        _mesh.startMesh();
        _tcpServer.start();
    }

    void StationContext::loop()
    {
        #ifdef LOG_HEAP
        if(millis() - _lastHeapLog > 1000)
        {
            Serial.printf("[MEMORY] Free: %i\n", ESP.getFreeHeap());
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
    
    LED& StationContext::getLed() { return _led; }
    Display& StationContext::getDisplay() { return _display; }
    TcpServer& StationContext::getTcpServer() { return _tcpServer; }
    NodeManager& StationContext::getNodeManager() { return _nodeManager; }
    ScriptManager& StationContext::getScriptManager() { return _scriptManager; }
    uint64_t StationContext::getStationId() const { return _stationId; }
}