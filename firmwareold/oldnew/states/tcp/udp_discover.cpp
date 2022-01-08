#include "udp_discover.hpp"

namespace SyncBlink
{
    void UdpDiscover::start(bool isServer)
    {
        _udp.begin(4210);
        _isServer = isServer;
    }

    void UdpDiscover::loop()
    {
        int packetSize = _udp.parsePacket();
        if (packetSize) 
        {
            char packet[255];
            int len = _udp.read(packet, 255);
            packet[len] = '\0';
            
            if(std::string(packet) == "syncPing" && _isServer)
            {
                _udp.beginPacket(_udp.remoteIP(), 4210);
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

        _udp.beginPacket(broadcastIp, 4210);
        _udp.write("syncPing");
        _udp.endPacket();
    }

    bool UdpDiscover::serverDiscovered(IPAddress& serverIp)
    {
        if(_serverDiscovered) serverIp = _serverIp;
        return _serverDiscovered;
    }
}