#include "node_context.hpp"

#include <EEPROM.h>

SyncBlink::NodeContext nodeContext;

void setup()
{
    Serial.begin(74880);
    EEPROM.begin(512);
    Serial.println("Starting SyncBlink Node ...");

    nodeContext.setup();  
    
    pinMode(LED_PIN, OUTPUT); 

    #ifdef MODE_PIN
    pinMode(MODE_PIN, INPUT);
    #endif

    #ifdef IS_STANDALONE
    pinMode(A0, INPUT);
    #endif 
}

void loop()
{
    nodeContext.loop();
}