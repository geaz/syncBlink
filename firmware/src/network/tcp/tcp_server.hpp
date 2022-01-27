#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcp_client.hpp"

#include <ESP8266WiFi.h>
#include <event/event_bus.hpp>
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
        TcpServer(EventBus& eventBus);

        void start();
        void loop();
        void broadcast(void* body, uint32_t bodySize, EventType eventType);

        uint32_t getClientsCount();

    private:
        void clearClients();
        void checkNewClients();
        void handleIncomingMessages();

        EventBus& _eventBus;
        WiFiServer _server = WiFiServer(81);
        std::vector<std::shared_ptr<TcpClient>> _clients;
    };
}

#endif // TCPSERVER_H