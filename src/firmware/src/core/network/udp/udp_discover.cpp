#include "udp_discover.hpp"

#include <string>

namespace SyncBlink
{
    void UdpDiscover::start(bool isServer)
    {
        _isServer = isServer;
        if(_isServer) _udp.begin(4210);
        else _udp.begin(4211);
    }

    void UdpDiscover::stop()
    {
        _udp.stop();
    }

    void UdpDiscover::loop()
    {
        int packetSize = _udp.parsePacket();
        if (packetSize)
        {
            char packet[255];
            int len = _udp.read(packet, 255);
            packet[len] = '\0';

            if (std::string(packet) == "syncPing" && _isServer)
            {
                Serial.println("[UDP] Received ping ...");
                _udp.beginPacket(_udp.remoteIP(), 4211);
                _udp.write("syncPong");
                _udp.endPacket();
                Serial.println("[UDP] Send pong!");
            }
            else if (std::string(packet) == "syncPong")
            {
                _serverDiscovered = true;
                _serverIp = _udp.remoteIP();
            }
        }
    }

    void UdpDiscover::ping()
    {
        IPAddress localIp = WiFi.localIP();
        IPAddress broadcastIp = IPAddress(localIp[0], localIp[1], localIp[2], 255);

        _udp.beginPacket(broadcastIp, 4210);
        _udp.write("syncPing");
        _udp.endPacket();
    }

    bool UdpDiscover::serverDiscovered(IPAddress& serverIp)
    {
        if (_serverDiscovered) serverIp = _serverIp;
        return _serverDiscovered;
    }
}