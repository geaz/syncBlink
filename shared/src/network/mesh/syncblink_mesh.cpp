#include "syncblink_mesh.hpp"

namespace SyncBlink
{
    uint64_t getId()
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
    
    SyncBlinkMesh::SyncBlinkMesh()
    {
        WiFi.mode(WIFI_STA);
        WiFi.persistent(false);
        WiFi.setAutoConnect(false);
        WiFi.setAutoReconnect(false);
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
    }

    void SyncBlinkMesh::startMesh()
    {
        Serial.println("Starting mesh ...");

        _ssid = SSID + " #1";
        WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
        WiFi.softAP(_ssid, Password, 1, false, 8);
        WiFi.mode(WIFI_AP_STA);

        _localIp = WiFi.localIP();
        Serial.println("Node AP IP: " + WiFi.softAPIP().toString());
        Serial.println("Node Local IP: " + WiFi.localIP().toString());
    }

    bool SyncBlinkMesh::tryJoinMesh()
    {
        bool connected = false;

        Serial.println("Scanning for SyncBlink Nodes ...");
        unsigned char foundNetworkCount = WiFi.scanNetworks();
        unsigned char nodeNr = 1;
        short nearestNode = -1;
        for (int i = 0; i < foundNetworkCount; ++i)
        {
            if (WiFi.SSID(i).startsWith(SSID))
            {
                if (nearestNode == -1 || (nearestNode != -1 && WiFi.RSSI(i) > WiFi.RSSI(nearestNode)))
                {
                    nearestNode = i;
                }
            }

            String ssid = WiFi.SSID(i);
            short foundNodeNr = ssid.substring(ssid.indexOf("#") + 1).toInt();
            if (foundNodeNr >= nodeNr)
                nodeNr = foundNodeNr + 1;
        }

        if (nearestNode != -1)
        {
            Serial.println("Connecting to '" + WiFi.SSID(nearestNode) + "' (15 sec Timeout)...");
            WiFi.begin(WiFi.SSID(nearestNode), Password);

            if(WiFi.waitForConnectResult(15000) == WL_CONNECTED)
            {
                Serial.println("Connected!");

                _ssid = SSID + " #" + String(nodeNr);
                WiFi.softAPConfig(IPAddress(192, 168, nodeNr, 1), IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
                WiFi.softAP(_ssid, Password, 1, false, 8);
                WiFi.mode(WIFI_AP_STA);

                Serial.println("Node AP IP: " + WiFi.softAPIP().toString());
                Serial.println("Node Local IP: " + WiFi.localIP().toString());
                connected = true;

                _parentIp = WiFi.gatewayIP();
                _localIp = WiFi.localIP();
            }
        }
        return connected;
    }

    bool SyncBlinkMesh::isConnected() const
    {
        return WiFi.isConnected();
    }

    bool SyncBlinkMesh::hasConnectedNodes() const
    {
        return WiFi.softAPgetStationNum() != 0;
    }

    IPAddress SyncBlinkMesh::getParentIp() const
    {
        return _parentIp;
    }

    IPAddress SyncBlinkMesh::getLocalIp() const
    {
        return _localIp;
    }
}