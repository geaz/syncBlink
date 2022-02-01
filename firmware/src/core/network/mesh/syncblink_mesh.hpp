#ifndef MESH_H
#define MESH_H

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
        bool hasConnectedNodes() const;
        IPAddress getParentIp() const;
        IPAddress getLocalIp() const;

    private:
        const char* _wifiSsid;
        const char* _wifiPw;

        String _ssid = SSID;
        IPAddress _parentIp = IPAddress(0, 0, 0, 0);
        IPAddress _localIp = IPAddress(0, 0, 0, 0);
    };
}

#endif // MESH_H