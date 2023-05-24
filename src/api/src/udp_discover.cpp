#include "udp_discover.hpp"

#include <array>
#include <iostream>

namespace SyncBlink
{
    namespace Api
    {
        UdpDiscover::UdpDiscover() : _socket(_ioService)
        {
        }

        UdpDiscover::~UdpDiscover()
        {
            _socket.shutdown(asio::socket_base::shutdown_both);
            _socket.close();
        }

        std::string UdpDiscover::ping()
        {
            for (auto event : infoMessageEvents.getEventHandlers())
                event.second("Sending ping, waiting for pong ...");

            asio::error_code err;
            if (!_open)
            {
                for (auto event : infoMessageEvents.getEventHandlers())
                    event.second("Socket not open. Opening ...");

                _socket.open(asio::ip::udp::v4(), err);
                if (checkError(err)) return "";

                _socket.set_option(asio::socket_base::broadcast(true), err);
                if (checkError(err)) return "";

                _socket.set_option(asio::ip::udp::socket::reuse_address(true), err);
                if (checkError(err)) return "";

                _socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), UdpPongPort), err);
                if (checkError(err)) return "";

                _open = true;

                for (auto event : infoMessageEvents.getEventHandlers())
                    event.second("Socket open!");
            }

            return receivePong();
        }

        std::string UdpDiscover::receivePong()
        {
            std::string hubIp = "";
            asio::ip::udp::endpoint server;
            std::array<char, 8> receivedMsg;

            auto writeHandler = [this, &server, &receivedMsg, &hubIp](const asio::error_code err, size_t sentBytes) { checkError(err); };

            auto readHandle = [this, &server, &receivedMsg, &hubIp](const asio::error_code err, size_t receivedBytes) {
                if (!checkError(err) && receivedBytes == 8 &&
                    std::string(receivedMsg.begin(), receivedMsg.begin() + receivedBytes) == "syncPong")
                {
                    for (auto event : infoMessageEvents.getEventHandlers())
                        event.second("Received pong from " + server.address().to_string());

                    hubIp = server.address().to_string();
                }
            };

            _socket.async_send_to(asio::buffer(_pingMsg.c_str(), _pingMsg.size()), _broadcastEndpoint, writeHandler);
            _socket.async_receive_from(asio::buffer(receivedMsg.data(), receivedMsg.size()), server, readHandle);

            std::thread abortIo([this, hubIp]() {
                // wait 5sec for pong receive - otherwise stop read
                auto start = std::chrono::system_clock::now();
                auto elapsed = std::chrono::system_clock::now() - start;
                auto elapsedSecs = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
                while (elapsedSecs < 5 && hubIp != "")
                {
                    elapsed = std::chrono::system_clock::now() - start;
                    elapsedSecs = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
                }
                if (hubIp == "") _ioService.stop();
            });

            _ioService.run();
            abortIo.join();
            _ioService.reset();

            if (hubIp == "")
            {
                for (auto event : infoMessageEvents.getEventHandlers())
                    event.second("No pong received!");
            }

            return hubIp;
        }

        bool UdpDiscover::checkError(asio::error_code err)
        {
            if (err.value() != 0)
            {
                for (auto event : errorMessageEvents.getEventHandlers())
                    event.second(err.message());
            }
            return err.value() != 0;
        }
    }
}