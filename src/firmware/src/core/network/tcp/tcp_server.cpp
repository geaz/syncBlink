#include "tcp_server.hpp"

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"
#include "core/network/get_id.hpp"
#include "tcp_stream_helper.hpp"

namespace SyncBlink
{
    TcpServer::TcpServer(MessageBus& messageBus) : _messageBus(messageBus)
    {
    }

    void TcpServer::start()
    {
        _server.begin();
    }

    void TcpServer::loop()
    {
        checkNewClients();
        handleIncomingClientMessages();
    }

    void TcpServer::broadcast(File file)
    {
        clearClients(); // make sure all disconnected clients got removed
        for (auto client : _clients)
        {
            file.seek(0);
            client->writeFile(file);
        }
    }

    void TcpServer::broadcast(std::vector<uint8_t> message)
    {
        clearClients(); // make sure all disconnected clients got removed
        for (auto client : _clients)
        {
            client->writeMessage(message);
        }
    }

    uint32_t TcpServer::getClientsCount()
    {
        return _clients.size();
    }

    void TcpServer::clearClients()
    {
        for (auto iter = _clients.begin(); iter != _clients.end();)
        {
            auto client = iter->get();
            // check if client is still connected to ap
            bool connectedToAp = false;
            struct station_info* statInfo = wifi_softap_get_station_info();
            while (statInfo != nullptr)
            {
                IPAddress clientIp = IPAddress((&statInfo->ip)->addr);
                if (clientIp == client->getRemoteIp())
                {
                    connectedToAp = true;
                    break;
                }
                statInfo = STAILQ_NEXT(statInfo, next);
            }
            wifi_softap_free_station_info();

            // Analyzers are not connected to the AP. Thats why we check the client AND the AP connection
            if (client->isWriteTimeout() || (!connectedToAp && !client->isConnected()))
            {
                // We explicity abandon without reset (this will also purge the unsent message memory)
                for (auto pcb = tcp_active_pcbs; pcb != nullptr; pcb = pcb->next)
                {
                    IPAddress ip = pcb->remote_ip;
                    if (ip == client->getRemoteIp())
                    {
                        tcp_abandon(pcb, 0);
                        break;
                    }
                }
                uint64_t streamId = client->getStreamId();
                _clients.erase(iter);

                if (streamId != 0)
                {
                    Serial.printf("[TCP SERVER] Client lost connection: %12llx (AP Connected: %i, Write Timeout: %i)\n",
                                  streamId, connectedToAp, client->isWriteTimeout());
                    _messageBus.trigger(Messages::MeshConnection{streamId, false});
                }
            }
            else
                iter++;
        }
    }

    void TcpServer::checkNewClients()
    {
        if (_server.hasClient())
        {
            _clients.push_back(std::make_shared<TcpClient>(_messageBus, _server.available()));
        }
    }

    void TcpServer::handleIncomingClientMessages()
    {
        for (auto& client : _clients)
        {
            MessagePackage package;
            if (TcpStreamHelper::messageAvailable(client->getWiFiClient(), package))
            {
                if (package.type == MessageType::MeshConnection)
                {
                    Messages::MeshConnection connectionMsg;
                    connectionMsg.loadPackage(package);

                    auto& nodeInfo = connectionMsg.nodeInfo;
                    if (connectionMsg.isConnected && nodeInfo.parentId == 0)
                    {
                        if (nodeInfo.isNode) nodeInfo.parentId = SyncBlink::getId();
                        client->setStreamId(connectionMsg.nodeId);
                    }
                    _messageBus.trigger(connectionMsg);
                }
                else
                    MessageBus::packageToBus(_messageBus, package);
            }
        }
    }
}