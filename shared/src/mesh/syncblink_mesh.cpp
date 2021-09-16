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
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
    }

    void SyncBlinkMesh::startMesh()
    {
        Serial.println("[MESH] Starting mesh ...");

        _ssid = SSID + " #1";
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
        WiFi.softAP(_ssid, Password, 1, false, 8);

        _localIp = WiFi.localIP();
        Serial.println("[MESH] Node AP IP: " + WiFi.softAPIP().toString());
        Serial.println("[MESH] Node Local IP: " + WiFi.localIP().toString());
    }

    bool SyncBlinkMesh::tryJoinMesh()
    {
        bool connected = false;

        Serial.println("[MESH] Scanning for SyncBlink Nodes ...");
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

                if(connectToNode == -1 || (foundSyncblinkNetworks > 2 && foundNodeNr > highestNodeNr))
                    connectToNode = i;

                if(foundNodeNr > highestNodeNr) highestNodeNr = foundNodeNr;
                if (foundNodeNr >= nodeNr) nodeNr = foundNodeNr + 1;
            }
        }

        if (connectToNode != -1)
        {
            Serial.println("[MESH] Connecting to '" + WiFi.SSID(connectToNode) + "' (30 sec Timeout)...");
            WiFi.begin(WiFi.SSID(connectToNode), Password);

            if(WiFi.waitForConnectResult(30000) == WL_CONNECTED)
            {
                Serial.println("[MESH] Connected!");

                _ssid = SSID + " #" + String(nodeNr);
                
                WiFi.softAPConfig(IPAddress(192, 168, nodeNr, 1), IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
                WiFi.softAP(_ssid, Password, 1, false, 8);
                WiFi.mode(WIFI_AP_STA);
                WiFi.setAutoReconnect(false);

                Serial.println("[MESH] Node AP IP: " + WiFi.softAPIP().toString());
                Serial.println("[MESH] Node Local IP: " + WiFi.localIP().toString());
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