#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    static const String SSID = "SyncBlink Network";
    static const String Password = "SyncBlinkSaysLetThereBeLight!";

    class SyncBlinkMesh
    {
    public:
        SyncBlinkMesh();

        void startMesh();
        bool tryJoinMesh();

        bool isConnected() const;
        bool hasConnectedNodes() const;
        IPAddress getParentIp() const;
        IPAddress getLocalIp() const;

    private:
        String _ssid = SSID;
        IPAddress _parentIp = IPAddress(0, 0, 0, 0);
        IPAddress _localIp = IPAddress(0, 0, 0, 0);
    };
}

#endif // WIFI_H