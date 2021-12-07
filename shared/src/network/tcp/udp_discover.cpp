#include "udp_discover.hpp"

namespace SyncBlink
{
    void UdpDiscover::start()
    {
        _udp.begin(4210);
    }

    void UdpDiscover::loop()
    {
        int packetSize = _udp.parsePacket();
        if (packetSize) 
        {
            char packet[255];
            int len = _udp.read(packet, 255);
            packet[len] = '\0';
            
            if(std::string(packet) == "syncPing")
            {
                _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
                _udp.write("syncPong");
                _udp.endPacket();
            }
            else if(std::string(packet) == "syncPong")
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

        _udp.beginPacket(broadcastIp, _udp.remotePort());
        _udp.write("syncPing");
        _udp.endPacket();
    }

    bool UdpDiscover::serverDiscovered(IPAddress& serverIp)
    {
        if(_serverDiscovered) serverIp = _serverIp;
        return _serverDiscovered;
    }
}