#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcp_client.hpp"
#include "event_bus.hpp"
#include "messages/message.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <vector>
#include <lwip/tcp.h>
#include <ESP8266WiFi.h>

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern struct tcp_pcb* tcp_active_pcbs;
extern "C" void tcp_abandon (struct tcp_pcb* pcb, int reset);

namespace SyncBlink
{
    struct ClientEvent
    {
        uint64_t NodeId;
        Client::MessageType MessageType;
        std::shared_ptr<Client::ConnectionMessage> ConnectionMessage;
    };

    class TcpServer
    {
    public:
        TcpServer(EventBus& eventBus);

        void start();
        void loop();
        void broadcast(void* body, uint32_t bodySize, Server::MessageType messageType);

        uint32_t getClientsCount();

    private:
        void clearClients();
        void checkNewClients();
        void handleIncomingMessages();

        EventBus& _eventBus;
        WiFiServer _server = WiFiServer(81);
        std::vector<TcpClient> _clients;
    };
}

#endif // TCPSERVER_H