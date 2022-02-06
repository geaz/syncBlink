#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcp_client.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/script_change.hpp"
#include "core/message/messages/mesh_update.hpp"
#include "core/message/messages/mesh_connection.hpp"

#include <ESP8266WiFi.h>
#include <lwip/tcp.h>
#include <vector>

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern struct tcp_pcb* tcp_active_pcbs;
extern "C" void tcp_abandon(struct tcp_pcb* pcb, int reset);

namespace SyncBlink
{
    class TcpServer :
        public MessageHandler<Messages::AnalyzerUpdate>,
        public MessageHandler<Messages::ScriptChange>,
        public MessageHandler<Messages::MeshUpdate>
    {
    public:
        TcpServer(MessageBus& messageBus);
        ~TcpServer();

        void start();
        void loop();

        uint32_t getClientsCount();
        
        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::ScriptChange& msg);
        void onMsg(const Messages::MeshUpdate& msg);

    private:
        void broadcast(const void* body, uint32_t bodySize, MessageType msgType);
        void clearClients();
        void checkNewClients();
        void handleIncomingMessages();

        MessageBus& _messageBus;
        WiFiServer _server = WiFiServer(81);
        std::vector<std::shared_ptr<TcpClient>> _clients;
        
        uint32_t _analyzerHandleId = 0;
        uint32_t _scriptHandleId = 0;
        uint32_t _meshUpdateHandleId = 0;
    };
}

#endif // TCPSERVER_H