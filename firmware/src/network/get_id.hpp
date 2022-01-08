#ifndef GETID_H
#define GETID_H

#include <cinttypes>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    inline uint64_t getId()
    {
        uint8_t mac[6];
        wifi_get_macaddr(STATION_IF, mac);
        
        uint64_t id = 0;
        for(uint8_t i = 0; i < 6; i++)
        {
            id += (uint64_t)mac[i] << (i*8);
        }
        return id;
    }
}

#endif // GETID_H