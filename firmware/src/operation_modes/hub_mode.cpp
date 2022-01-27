#include "hub_mode.hpp"

#include "views/splash_view.cpp"

namespace SyncBlink
{
    HubMode::HubMode(Config& config)
        : _config(config), _mesh(_config.Values["wifi_ssid"], _config.Values["wifi_pw"]),
          _scriptManager(_eventBus, _config), _stateContext(_eventBus, _led, _display, _scriptManager)
    {
    }

    void HubMode::setup()
    {
        _display.init();
        _display.setView(std::make_shared<SyncBlink::SplashView>());
        _display.loop();

        _led.setup(_config.Values["led_count"]);

        _mesh.connectWifi();
        _mesh.startMesh();
    }

    void HubMode::loop()
    {
        _display.setLeftStatus("");
        _display.setRightStatus(WiFi.localIP().toString().c_str());

        _stateContext.loop();

        _led.loop();
        _display.loop();
    }
}