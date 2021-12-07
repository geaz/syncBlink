#ifndef UDPDISCOVERY_H
#define UDPDISCOVERY_H

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    class UdpDiscover
    {
    public:
        void start();
        void loop();
        void ping();

        bool serverDiscovered(IPAddress& serverIp);

    private:
        WiFiUDP _udp;
        bool _serverDiscovered;
        IPAddress _serverIp;
    };
}

#endif // UDPDISCOVERY_H