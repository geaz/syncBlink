#include "node_context.hpp"

SyncBlink::NodeContext nodeContext;

void setup()
{
    Serial.begin(74880);
    Serial.println("Starting SyncBlink Node ...");

    nodeContext.setup();

    pinMode(LED_PIN, OUTPUT);    
}

void loop()
{
    nodeContext.loop();
}