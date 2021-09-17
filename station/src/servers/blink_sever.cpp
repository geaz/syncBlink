#include "blink_server.hpp"

namespace SyncBlink
{
    BlinkServer::BlinkServer(LED& led, NodeManager& nodeManager) : _led(led), _nodeManager(nodeManager)
    {
        _tcpServer.messageEvents
            .addEventHandler([this](Message message) { onSocketServerCommandReceived(message); });
        _tcpServer.serverDisconnectionEvents
            .addEventHandler([this](uint64_t nodeId) { onMeshDisconnection(nodeId); });
    }

    void BlinkServer::start()
    {
        _tcpServer.start();
    }

    void BlinkServer::loop()
    {
        _tcpServer.loop();
    }

    TcpServer& BlinkServer::getTcpServer()
    {
        return _tcpServer;
    }

    void BlinkServer::onSocketServerCommandReceived(Message message)
    {
        switch (message.type)
        {
            case Client::MESH_CONNECTION:
            {
                auto conMessage = Message::as<Client::ConnectionMessage>(message);
                if(conMessage.isAnalyzer && !conMessage.isNode)
                {
                    Serial.printf("[MESH] New Analyzer connected: %s\n", conMessage.nodeLabel);
                }
                else
                {
                    Serial.printf("[MESH] New Node: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                        conMessage.nodeId, conMessage.ledCount,
                        conMessage.parentId, conMessage.majorVersion, conMessage.minorVersion);
                }

                _nodeManager.addNode(conMessage);

                Server::UpdateMessage updateMessage = { _nodeManager.getActiveAnalyzer(), _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
                break;
            }
            case Client::MESH_DISCONNECTION:
            {
                auto nodeId = Message::as<uint64_t>(message);
                onMeshDisconnection(nodeId);

                Serial.printf("[MESH] Node disconnected: %12llx\n", nodeId);

                Server::UpdateMessage updateMessage = { _nodeManager.getActiveAnalyzer(), _led.getLedCount(), 1, _nodeManager.getTotalLedCount(), _nodeManager.getTotalNodeCount() };
                _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
                break;
            }
            // Handled in different station states
            case Client::EXTERNAL_ANALYZER:
            case Client::SCRIPT_DISTRIBUTED:
                break;
        }
    }

    void BlinkServer::onMeshDisconnection(uint64_t nodeId)
    {
        _nodeManager.removeNode(nodeId);
    }
}