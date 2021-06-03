#include "node_context.hpp"

#include <EEPROM.h>

SyncBlink::NodeContext nodeContext;

void setup()
{
    Serial.begin(74880);
    Serial.println("Starting SyncBlink Node ...");

    EEPROM.begin(512);
    pinMode(LED_PIN, OUTPUT); 

    nodeContext.setup();   
}

void loop()
{
    nodeContext.loop();
}