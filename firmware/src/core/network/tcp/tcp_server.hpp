#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcp_client.hpp"
#include "core/event/event_bus.hpp"
#include "core/event/events/analyzer_update_event.hpp"
#include "core/event/events/script_change_event.hpp"
#include "core/event/events/mesh_update_event.hpp"

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
        public EventHandler<Events::AnalyzerUpdateEvent>,
        public EventHandler<Events::ScriptChangeEvent>,
        public EventHandler<Events::MeshUpdateEvent>
    {
    public:
        TcpServer(EventBus& eventBus);
        ~TcpServer();

        void start();
        void loop();

        uint32_t getClientsCount();
        
        void onEvent(const Events::AnalyzerUpdateEvent& event);
        void onEvent(const Events::ScriptChangeEvent& event);
        void onEvent(const Events::MeshUpdateEvent& event);

    private:
        void broadcast(const void* body, uint32_t bodySize, EventType eventType);
        void clearClients();
        void checkNewClients();
        void handleIncomingMessages();

        EventBus& _eventBus;
        WiFiServer _server = WiFiServer(81);
        std::vector<std::shared_ptr<TcpClient>> _clients;
        
        uint32_t _analyzerEventHandleId = 0;
        uint32_t _scriptEventHandleId = 0;
        uint32_t _meshUpdateEventHandleId = 0;
    };
}

#endif // TCPSERVER_H