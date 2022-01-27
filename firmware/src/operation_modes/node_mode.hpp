#ifndef NODEMODE_H
#define NODEMODE_H

#include "config/config.hpp"
#include "operation_mode.hpp"

#include <ArduinoJson.h>
#include <led.hpp>

namespace SyncBlink
{
    class NodeMode : public OperationMode
    {
    public:
        NodeMode(Config& config);

        void setup();
        void loop();

    private:
        void checkConnection();

        Config& _config;
        LED _led;
    };
}

#endif // NODEMODE_H