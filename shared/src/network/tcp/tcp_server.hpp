#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcp_client.hpp"
#include "event_registration.hpp"
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
    typedef std::function<void(Message message)> ServerMessageEvent;
    typedef std::function<void(Client::ConnectionMessage)> ServerConnectionEvent;
    typedef std::function<void(uint64_t nodeId)> ServerDisconnectionEvent;    
    typedef std::function<void(AudioAnalyzerMessage message)> ServerExternalAnalyzerEvent;
    typedef std::function<void()> ServerScriptDistributedEvent;

    class TcpServer
    {
    public:
        void start();
        void loop();
        void broadcast(void* body, uint32_t bodySize, Server::MessageType messageType);

        uint32_t getClientsCount();

        EventRegistration<ServerMessageEvent> serverMessageEvents;
        EventRegistration<ServerConnectionEvent> serverConnectionEvents;
        EventRegistration<ServerDisconnectionEvent> serverDisconnectionEvents;
        EventRegistration<ServerExternalAnalyzerEvent> serverExternalAnalyzerEvent;
        EventRegistration<ServerScriptDistributedEvent> serverScriptDistributedEvents;

    private:
        void clearClients();
        void checkNewClients();
        void handleIncomingMessages();

        WiFiServer _server = WiFiServer(81);
        std::vector<TcpClient> _clients;
    };
}

#endif // TCPSERVER_H