#include "node_mode.hpp"
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    NodeMode::NodeMode(Config& config) : _config(config) { }

    void NodeMode::setup()
    {
        _led.setup(_config.Values["led_count"]);
    }

    void NodeMode::loop()
    {
        checkConnection();

        _led.loop();
    }

    void NodeMode::checkConnection()
    {
        if(WiFi.isConnected()) return;
        _led.showNow(Colors::Blue);

        // Try connect to hub

    }
}