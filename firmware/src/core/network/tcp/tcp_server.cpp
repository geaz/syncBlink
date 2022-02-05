#include "tcp_server.hpp"

#include "message.hpp"
#include "core/network/get_id.hpp"
#include "core/event/event_types.hpp"
#include "core/event/events/mesh_connection_event.hpp"

namespace SyncBlink
{
    TcpServer::TcpServer(EventBus& eventBus) : _eventBus(eventBus)
    {
        _analyzerEventHandleId = _eventBus.addEventHandler<Events::AnalyzerUpdateEvent>(this);
        _scriptEventHandleId = _eventBus.addEventHandler<Events::ScriptChangeEvent>(this);
        _meshUpdateEventHandleId = _eventBus.addEventHandler<Events::MeshUpdateEvent>(this);
    }

    TcpServer::~TcpServer()
    {
        _eventBus.removeEventHandler(_analyzerEventHandleId);
        _eventBus.removeEventHandler(_scriptEventHandleId);
        _eventBus.removeEventHandler(_meshUpdateEventHandleId);
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

    uint32_t TcpServer::getClientsCount()
    {
        return _clients.size();
    }

    void TcpServer::onEvent(const Events::AnalyzerUpdateEvent& event)
    {
        broadcast(&event, sizeof(event), EventType::AnalyzerUpdateEvent);
    }

    void TcpServer::onEvent(const Events::ScriptChangeEvent& event)
    {
        broadcast(&event, sizeof(event), EventType::ScriptChangeEvent);
    }

    void TcpServer::onEvent(const Events::MeshUpdateEvent& event)
    {
        broadcast(&event, sizeof(event), EventType::MeshUpdateEvent);
    }

    void TcpServer::broadcast(const void* body, uint32_t bodySize, EventType eventType)
    {
        auto packet = Message::toMessagePacket(body, bodySize, eventType);
        for (auto client : _clients)
        {
            client->writeMessage(packet);
        }
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
            else iter++;
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
        for(auto& client : _clients)
        {
            Message message;
            if(Message::available(client->getWiFiClient(), message))
            {
                switch(message.type)
                {
                    case EventType::MeshConnectionEvent:
                    {
                        auto event = message.as<Events::MeshConnectionEvent>();
                        auto& nodeInfo = event.nodeInfo;
                        if(event.isConnected && nodeInfo.parentId == 0)
                        {
                            if(nodeInfo.isNode) nodeInfo.parentId = SyncBlink::getId();
                            client->setStreamId(event.nodeId);

                            if(nodeInfo.isAnalyzer && !nodeInfo.isNode)
                            {
                                Serial.printf("[TCP SERVER] New Analyzer connected: %s\n", nodeInfo.nodeLabel.c_str());
                            }
                            else
                            {
                                Serial.printf("[TCP SERVER] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                                    event.nodeId, nodeInfo.ledCount,
                                    nodeInfo.parentId, nodeInfo.majorVersion, nodeInfo.minorVersion);
                            }
                        }
                        else if(!event.isConnected)
                        {
                            Serial.printf("[TCP SERVER] Node disconnected: %12llx\n", event.nodeId);
                        }
                        _eventBus.trigger(event);
                        break;
                    }
                    case EventType::AnalyzerUpdateEvent:
                    {
                        auto analyzerEvent = message.as<Events::AnalyzerUpdateEvent>();
                        _eventBus.trigger(analyzerEvent);
                        break;
                    }
                }
            }
        }
    }
}