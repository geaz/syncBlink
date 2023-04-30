#include "tcp_server.hpp"

#include "tcp_stream_helper.hpp"
#include "core/message/message.hpp"
#include "core/message/message_types.hpp"
#include "core/network/get_id.hpp"

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
        clearClients();
        checkNewClients();
        handleIncomingClientMessages();
    }

    void TcpServer::broadcast(std::vector<uint8_t> message)
    {
        for (auto client : _clients)
        {
            client.writeMessage(message);
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
            // check if client is still connected to ap
            bool connectedToAp = false;
            struct station_info* statInfo = wifi_softap_get_station_info();
            while (statInfo != nullptr)
            {
                IPAddress clientIp = IPAddress((&statInfo->ip)->addr);
                if (clientIp == iter->getRemoteIp())
                {
                    connectedToAp = true;
                    break;
                }
                statInfo = STAILQ_NEXT(statInfo, next);
            }
            wifi_softap_free_station_info();

            // Analyzers are not connected to the AP. Thats why we check the client AND the AP connection
            if (iter->isWriteTimeout() || (!connectedToAp && !iter->isConnected()))
            {
                if (iter->getStreamId() != 0)
                {
                    Serial.printf("[TCP SERVER] Client lost connection: %12llx (AP Connected: %i, Write Timeout: %i)\n",
                                  iter->getStreamId(), connectedToAp, iter->isWriteTimeout());

                    Messages::MeshConnection msg;
                    msg.nodeId = iter->getStreamId();
                    msg.isConnected = false;

                    _messageBus.trigger(msg);
                }
                iter->flush();
                iter->stop();

                // We explicity abandon without reset (this will also purge the unsent message memory)
                for (auto pcb = tcp_active_pcbs; pcb != nullptr; pcb = pcb->next)
                {
                    IPAddress ip = pcb->remote_ip;
                    if (ip == iter->getRemoteIp())
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
            _clients.push_back(TcpClientHandle(_server.available()));
        }
    }

    void TcpServer::handleIncomingClientMessages()
    {
        for (auto& client : _clients)
        {
            MessagePackage package;
            if (TcpStreamHelper::messageAvailable(client.getWiFiClient(), package))
            {
                #ifdef DEBUG_TCP
                Serial.println("[TCP SERVER] Got message!");                   
                #endif
                
                if(package.type == MessageType::MeshConnection)
                {
                    Messages::MeshConnection connectionMsg;
                    connectionMsg.loadPackage(package);

                    auto& nodeInfo = connectionMsg.nodeInfo;
                    if (connectionMsg.isConnected && nodeInfo.parentId == 0)
                    {
                        if (nodeInfo.isNode) nodeInfo.parentId = SyncBlink::getId();
                        client.setStreamId(connectionMsg.nodeId);
                    }

                    #ifdef DEBUG_TCP
                    NodeInfo info = connectionMsg.nodeInfo;
                    if(info.isAnalyzer && !info.isNode)
                    {
                        Serial.printf("[TCP SERVER] New Analyzer connected: %s\n", info.nodeLabel.c_str());
                    }
                    else
                    {
                        Serial.printf("[TCP SERVER] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                            connectionMsg.nodeId, info.ledCount, info.parentId, info.majorVersion, info.minorVersion);
                    }                    
                    #endif

                    _messageBus.trigger(connectionMsg);
                }
                else MessageBus::packageToBus(_messageBus, package);
            }
        }
    }
}