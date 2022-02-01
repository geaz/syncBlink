#include "display_module.hpp"

#include "core/views/splash_view.cpp"

#include <ESP8266WiFi.h>

namespace SyncBlink
{
    DisplayModule::DisplayModule(EventBus& eventBus) : _eventBus(eventBus)
    {   
        _displayCommandHandleId = _eventBus.addEventHandler<Commands::SetDisplay>(this);
    }
    
    DisplayModule::~DisplayModule()
    {
        _eventBus.removeEventHandler(_displayCommandHandleId);
    }

    void DisplayModule::setup()
    { 
        _display.init();
        _display.setView(std::make_shared<SplashView>());
        _display.loop();
    }

    void DisplayModule::loop()
    {     
        _display.setRightStatus(WiFi.localIP().toString().c_str());
        _display.loop();
    }

    void DisplayModule::onEvent(const Commands::SetDisplay& command)
    {
        _display.setView(command.view);
        _display.setLeftStatus(command.leftStatus);
    }
}