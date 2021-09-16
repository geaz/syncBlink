#ifndef APISERVER_H
#define APISERVER_H

#include <tcp/tcp_server.hpp>
#include <messages/api_messages.hpp>

namespace SyncBlink
{
    class ApiServer
    {
        public:
            ApiServer(TcpServer& blinkServer);
            void start();
            void loop();

        private:
            void onSocketServerCommandReceived(Message message);

            TcpServer& _blinkServer;
    };
}

#endif // APISERVER_H