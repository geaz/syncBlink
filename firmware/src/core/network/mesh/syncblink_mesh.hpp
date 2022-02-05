#ifndef MESH_H
#define MESH_H

#include "core/network/udp/udp_discover.hpp"

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    static const String SSID = "SyncBlink Network";
    static const String Password = "syncBlink";

    class SyncBlinkMesh
    {
    public:
        SyncBlinkMesh(const char* wifiSsid, const char* wifiPw);

        void connectWifi();
        void startMesh();
        bool tryJoinMesh();

        bool isConnected() const;
        bool isConnectedToMeshWifi() const;
        bool hasConnectedNodes() const;
        IPAddress getParentIp() const;
        IPAddress getLocalIp() const;

    private:
        const char* _wifiSsid;
        const char* _wifiPw;

        UdpDiscover _udpDiscover;

        String _ssid = SSID;
        IPAddress _parentIp = IPAddress(0, 0, 0, 0);
        IPAddress _localIp = IPAddress(0, 0, 0, 0);
        
        bool _connectedToMeshWiFi = true;
    };
}

#endif // MESH_H