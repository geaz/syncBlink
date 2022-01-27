#ifndef HUBMODE_H
#define HUBMODE_H

#include "config/config.hpp"
#include "event/event_bus.hpp"
#include "network/mesh/syncblink_mesh.hpp"
#include "operation_mode.hpp"
#include "scripting/script_manager.hpp"
#include "states/state_context.hpp"

#include <ArduinoJson.h>
#include <display.hpp>
#include <led.hpp>

namespace SyncBlink
{
    class HubMode : public OperationMode
    {
    public:
        HubMode(Config& config);

        void setup();
        void loop();

    private:
        LED _led;
        Config& _config;
        SyncBlinkMesh _mesh;

        Display _display;
        EventBus _eventBus;
        ScriptManager _scriptManager;
        StateContext _stateContext;
    };
}

#endif // HUBMODE_H