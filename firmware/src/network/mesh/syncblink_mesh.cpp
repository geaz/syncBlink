#include "syncblink_mesh.hpp"

namespace SyncBlink
{
    SyncBlinkMesh::SyncBlinkMesh(const char* wifiSsid, const char* wifiPw) : _wifiSsid(wifiSsid), _wifiPw(wifiPw)
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
    }

    void SyncBlinkMesh::connectWifi()
    {
        Serial.println("[WIFI] Connecting to Network ...");

        if (_wifiSsid != nullptr)
        {
            WiFi.begin(_wifiSsid, _wifiPw);

            Serial.println("[WIFI] Waiting for Wifi to connect (30 sec timeout)...");
            if (WiFi.waitForConnectResult(30000) == WL_CONNECTED)
            {
                Serial.println("[WIFI] Connected!");
            }
            else
            {
                Serial.println("[WIFI] Couldn't connect to network!");
                WiFi.disconnect();
            }
        }
    }

    void SyncBlinkMesh::startMesh()
    {
        Serial.println("[WIFI] Starting mesh ...");

        _ssid = SSID + " #1";
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
        WiFi.softAP(_ssid, Password, 1, false, 8);

        _localIp = WiFi.localIP();
        Serial.println("[WIFI] Node AP IP: " + WiFi.softAPIP().toString());
        Serial.println("[WIFI] Node Local IP: " + WiFi.localIP().toString());
    }

    bool SyncBlinkMesh::tryJoinMesh()
    {
        bool connected = false;

        Serial.println("[WIFI] Scanning for SyncBlink Nodes ...");
        uint8_t foundSyncblinkNetworks = 0;
        uint8_t foundNetworkCount = WiFi.scanNetworks();

        uint8_t nodeNr = 1;
        uint8_t highestNodeNr = 1;
        int8_t connectToNode = -1;

        for (int i = 0; i < foundNetworkCount; ++i)
        {
            if (WiFi.SSID(i).startsWith(SSID))
            {
                foundSyncblinkNetworks++;

                // We seek for an evenly distributed mesh
                // Thats why we always connect to the "highest node number", if we found
                // more than two nodes in range.

                String ssid = WiFi.SSID(i);
                short foundNodeNr = ssid.substring(ssid.indexOf("#") + 1).toInt();

                if ((connectToNode == -1 || foundSyncblinkNetworks > 2) && foundNodeNr > highestNodeNr)
                    connectToNode = i;

                if (foundNodeNr > highestNodeNr)
                    highestNodeNr = foundNodeNr;
                if (foundNodeNr >= nodeNr)
                    nodeNr = foundNodeNr + 1;
            }
        }

        if (connectToNode != -1)
        {
            Serial.println("[WIFI] Connecting to '" + WiFi.SSID(connectToNode) + "' (30 sec Timeout)...");
            WiFi.begin(WiFi.SSID(connectToNode), Password);

            if (WiFi.waitForConnectResult(30000) == WL_CONNECTED)
            {
                Serial.println("[WIFI] Connected!");

                _ssid = SSID + " #" + String(nodeNr);

                WiFi.softAPConfig(IPAddress(192, 168, nodeNr, 1), IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
                WiFi.softAP(_ssid, Password, 1, false, 8);
                WiFi.mode(WIFI_AP_STA);
                WiFi.setAutoReconnect(false);

                Serial.println("[WIFI] Node AP IP: " + WiFi.softAPIP().toString());
                Serial.println("[WIFI] Node Local IP: " + WiFi.localIP().toString());
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