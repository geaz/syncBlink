#include "tcp_server.hpp"

#include "message.hpp"
#include "network/get_id.hpp"

#include <event/events/mesh_connection_event.hpp>

namespace SyncBlink
{
    TcpServer::TcpServer(EventBus& eventBus) : _eventBus(eventBus)
    {
    }

    void TcpServer::start()
    {
        _server.begin();
    }

    void TcpServer::loop()
    {
        clearClients();
        checkNewClients();
        handleIncomingMessages();
    }

    void TcpServer::broadcast(void* body, uint32_t bodySize, EventType eventType)
    {
        auto packet = Message::toMessagePacket(body, bodySize, eventType);
        for (std::shared_ptr<TcpClient> client : _clients)
        {
            client->writeMessage(packet);
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
                if (client->getStreamId() != 0)
                {
                    Serial.printf("[TCP SERVER] Client lost connection: %12llx (AP Connected: %i, Write Timeout: %i)\n",
                                  client->getStreamId(), connectedToAp, client->isWriteTimeout());
                    _eventBus.trigger<Events::MeshConnectionEvent>({client->getStreamId(), false});
                }
                client->flush();
                client->stop();

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

                _clients.erase(iter);
            }
            else
                iter++;
        }
    }

    void TcpServer::checkNewClients()
    {
        if (_server.hasClient())
        {
            _clients.push_back(std::make_shared<TcpClient>(_eventBus, _server.available()));
        }
    }

    void TcpServer::handleIncomingMessages()
    {
    }
}