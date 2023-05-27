#ifndef SYNCBLINKAPIUDP_H
#define SYNCBLINKAPIUDP_H

#include "event_registration.hpp"

#include <asio.hpp>
#include <functional>
#include <string>

namespace SyncBlink
{
    namespace Api
    {
        typedef std::function<void(std::string message)> ErrorMessageEvent;
        typedef std::function<void(std::string message)> InfoMessageEvent;

        static const int UdpPingPort = 4210;
        static const int UdpPongPort = 4211;

        class UdpDiscover
        {
        public:
            UdpDiscover();
            ~UdpDiscover();

            std::string ping();

            EventRegistration<ErrorMessageEvent> errorMessageEvents;
            EventRegistration<InfoMessageEvent> infoMessageEvents;

        private:
            std::string receivePong();
            bool checkError(asio::error_code err);

            bool _open = false;
            asio::io_service _ioService;
            asio::ip::udp::socket _socket;

            std::string _pingMsg = "syncPing";
            asio::ip::udp::endpoint _broadcastEndpoint =
                asio::ip::udp::endpoint(asio::ip::address_v4::broadcast(), UdpPingPort);
        };
    }
}

#endif // SYNCBLINKAPIUDP_H