#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "core/message/message_bus.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/mesh_connection.hpp"
#include "tcp_client.hpp"

#include <ESP8266WiFi.h>
#include <lwip/tcp.h>
#include <vector>

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern struct tcp_pcb* tcp_active_pcbs;
extern "C" void tcp_abandon(struct tcp_pcb* pcb, int reset);

namespace SyncBlink
{
    class TcpServer
    {
    public:
        TcpServer(MessageBus& messageBus);

        void start();
        void loop();
        void broadcast(std::vector<uint8_t> message);

        uint32_t getClientsCount();

    private:
        void clearClients();
        void checkNewClients();
        void handleIncomingClientMessages();

        MessageBus& _messageBus;
        WiFiServer _server = WiFiServer(81);
        std::vector<std::shared_ptr<TcpClient>> _clients;
    };
}

#endif // TCPSERVER_H