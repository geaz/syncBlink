#include "node_context.hpp"

#include <ESP8266httpUpdate.h>
#include <shared_constants.hpp>

namespace SyncBlink
{
    void NodeContext::setup()
    {
        _led.setup(_nodeRom.getLedCount());
        _led.showNow(SyncBlink::Cyan);
        _nodeConnector.connectToMesh();
        _nodeRom.setup(_nodeConnector.getTcpClient(), _nodeConnector.getTcpServer());
    }

    void NodeContext::loop()
    {
        checkModeButton();
        
        _led.loop();
        _nodeAnalyzer.loop();
        _nodeConnector.loop();
        if(!_nodeConnector.isConnected() && !_nodeAnalyzer.runsStandalone())
        {
            Serial.print("Going to sleep ...\n");
            _led.blinkNow(Red);
            _led.showNow(SyncBlink::Black);
            ESP.deepSleep(SleepSeconds * 1000000);
        }
    }

    void NodeContext::checkModeButton()
    {
        if(!_hasModeButton || _lastButtonUpdate + 100 > millis()) return;

        char buttonVal = digitalRead(_modePin);
        if(buttonVal == HIGH && _lastButtonVal == LOW)
        {
            _nodeAnalyzer.toggleLightMode();
        }
        _lastButtonVal = buttonVal;
        _lastButtonUpdate = millis();
    }
}