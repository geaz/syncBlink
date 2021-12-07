#include "node_connector.hpp"

namespace SyncBlink
{
    NodeConnector::NodeConnector(NodeRom& nodeRom, LED& led) : _nodeRom(nodeRom), _led(led)
    {
        registerEvents();
    }

    void NodeConnector::loop()
    {
        _tcpClient.loop();
        _tcpServer.loop();
    }

    bool NodeConnector::connectToMesh()
    {
        if(_nodeRom.getSSID() != "" && _nodeRom.getWifiPw() != "")
        {
            Serial.print("Try connecting to wifi ...\n");
            WiFi.begin(_nodeRom.getSSID().c_str(), _nodeRom.getWifiPw().c_str());
            if(WiFi.waitForConnectResult(30000) == WL_CONNECTED)
            {
                Serial.printf("Connected to Wifi! Starting operation (v%i.%i)...\n", VERSIONMAJOR, VERSIONMINOR);
                _connectedToMeshWiFi = false;
                _udpDiscover.ping();

                uint64_t start = millis();
                while(!_udpDiscover.serverDiscovered(_serverIP) && start + 5000 > millis())
                { 
                    delay(100);
                }
                
                _tcpClient.start(_serverIP.toString(), 81);
                _tcpServer.start();
            }
        }
        
        if(!WiFi.isConnected() && _mesh.tryJoinMesh())
        {
            _connectedToMeshWiFi = true;
            Serial.printf("Connected to SyncBlink mesh! Starting operation (v%i.%i)...\n", VERSIONMAJOR, VERSIONMINOR);
            _serverIP = _mesh.getParentIp();
        }

        if(_serverIP != IPAddress(0,0,0,0))
        {
            Serial.print("Station found. Connecting TCP ...\n");
            _tcpClient.start(_serverIP.toString(), 81);
            _tcpServer.start();
        }
        else Serial.print("Station NOT found!");

        return WiFi.isConnected() && _tcpClient.isConnected();
    }

    bool NodeConnector::isConnected()
    {
        return WiFi.isConnected() && !_tcpClient.isDiscontinued();
    }
    
    uint32_t NodeConnector::getMeshLedCount() { return _meshLedCount; }
    uint32_t NodeConnector::getMeshNodeCount() { return _meshNodeCount; }
    uint32_t NodeConnector::getPrevLedCount() { return _previousLedCount; }
    uint32_t NodeConnector::getPrevNodeCount() { return _previousNodeCount; }
    TcpClient& NodeConnector::getTcpClient() { return _tcpClient; }
    TcpServer& NodeConnector::getTcpServer() { return _tcpServer; }

    void NodeConnector::registerEvents()
    {
        _tcpClient
            .pingEvents
            .addEventHandler([this](uint64_t nodeId) { if(nodeId == SyncBlink::getId()) _led.blinkNow(Yellow, 5); else _tcpServer.broadcast(&nodeId, sizeof(nodeId), Server::PING_NODE);});
        _tcpClient
            .connectionEvents
            .addEventHandler([this](bool connected) { onSocketClientConnectionChanged(connected); });
        _tcpClient
            .meshUpdateEvents
            .addEventHandler([this](Server::UpdateMessage message) { onMeshUpdateReceived(message); });
        _tcpServer
            .serverDisconnectionEvents
            .addEventHandler([this](uint64_t nodeId) { _tcpClient.sendMessage(&nodeId, sizeof(nodeId), Client::MESH_DISCONNECTION); });
    }    

    void NodeConnector::onSocketClientConnectionChanged(bool connected)
    {
         if(connected)
         {
            _led.blinkNow(Green);

            Client::ConnectionMessage message = { false, IS_STANDALONE, true, _connectedToMeshWiFi, SyncBlink::getId(), 0, _led.getLedCount(), VERSIONMAJOR, VERSIONMINOR };
            memcpy(&message.nodeLabel[0], &_nodeRom.getNodeLabel()[0], _nodeRom.getNodeLabel().size());

            _tcpClient.sendMessage(&message, sizeof(message), Client::MESH_CONNECTION);
         }
         else _led.blinkNow(Red); 
    }

    void NodeConnector::onMeshUpdateReceived(Server::UpdateMessage message)
    {
        _previousLedCount = message.routeLedCount;
        _previousNodeCount = message.routeNodeCount;
        _meshLedCount = message.meshLedCount;
        _meshNodeCount = message.meshNodeCount;   

        if(_tcpServer.getClientsCount() != 0)
        {
            message.routeNodeCount++;
            message.routeLedCount += _nodeRom.getLedCount();
            _tcpServer.broadcast(&message, sizeof(message), Server::MESH_UPDATE);
        }
        else
        {
            _tcpClient.sendMessage(0, 0, Client::MESH_UPDATED);
        }
    }
}