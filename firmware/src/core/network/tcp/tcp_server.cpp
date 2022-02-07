#include "tcp_server.hpp"

#include "core/message/message.hpp"
#include "core/network/get_id.hpp"
#include "core/message/message_types.hpp"

namespace SyncBlink
{
    TcpServer::TcpServer(MessageBus& messageBus) : _messageBus(messageBus)
    {
        _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(this);
        _scriptHandleId = _messageBus.addMsgHandler<Messages::ScriptChange>(this);
        _meshUpdateHandleId = _messageBus.addMsgHandler<Messages::MeshUpdate>(this);
    }

    TcpServer::~TcpServer()
    {
        _messageBus.removeMsgHandler(_analyzerHandleId);
        _messageBus.removeMsgHandler(_scriptHandleId);
        _messageBus.removeMsgHandler(_meshUpdateHandleId);
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

    void TcpServer::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        broadcast(msg.toPackage());
    }

    void TcpServer::onMsg(const Messages::ScriptChange& msg)
    {
        broadcast(msg.toPackage());
    }

    void TcpServer::onMsg(const Messages::MeshUpdate& msg)
    {
        broadcast(msg.toPackage());
    }

    void TcpServer::broadcast(std::vector<uint8_t> message)
    {
        for (auto client : _clients)
        {
            client->writeMessage(message);
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

                    Messages::MeshConnection msg;
                    msg.nodeId = client->getStreamId();
                    msg.isConnected = false;

                    _messageBus.trigger(msg);
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
            _clients.push_back(std::make_shared<TcpClient>(_messageBus, _server.available()));
        }
    }

    void TcpServer::handleIncomingMessages()
    {
        for(auto& client : _clients)
        {
            MessagePackage package;
            if(MessagePackage::available(client->getWiFiClient(), package))
            {
                switch(package.type)
                {
                    case MessageType::MeshConnection:
                    {
                        Messages::MeshConnection connectionMsg;
                        connectionMsg.loadPackage(package);

                        auto& nodeInfo = connectionMsg.nodeInfo;
                        if(connectionMsg.isConnected && nodeInfo.parentId == 0)
                        {
                            if(nodeInfo.isNode) nodeInfo.parentId = SyncBlink::getId();
                            client->setStreamId(connectionMsg.nodeId);
                        }
                        
                        _messageBus.trigger(connectionMsg);
                        break;
                    }
                    case MessageType::AnalyzerUpdate:
                    {
                        Messages::AnalyzerUpdate analyzerMsg;
                        analyzerMsg.loadPackage(package);

                        _messageBus.trigger(analyzerMsg);
                        break;
                    }
                }
            }
        }
    }
}