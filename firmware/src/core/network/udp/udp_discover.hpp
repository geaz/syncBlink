#ifndef UDPDISCOVERY_H
#define UDPDISCOVERY_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace SyncBlink
{
    class UdpDiscover
    {
    public:
        void start(bool isServer);
        void stop();
        void loop();
        void ping();

        bool serverDiscovered(IPAddress& serverIp);

    private:
        WiFiUDP _udp;
        bool _isServer;
        bool _serverDiscovered;
        IPAddress _serverIp;
    };
}

#endif // UDPDISCOVERY_H