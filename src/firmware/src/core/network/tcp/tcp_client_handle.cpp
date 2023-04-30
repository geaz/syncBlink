#include "tcp_client_handle.hpp"

namespace SyncBlink
{
    TcpClientHandle::TcpClientHandle(WiFiClient client) : _client(client)
    {
        _client.setNoDelay(true);
    }

    void TcpClientHandle::writeMessage(std::vector<uint8_t> message)
    {
        if (_client.connected())
        {
            long started = millis();
            uint8_t* messagePtr = &message[0];
            uint32_t messageSize = message.size();
            while (messageSize > 0)
            {
                if (millis() - started > SocketWriteTimeout)
                {
                    _writeTimeout = true;
                    break;
                }

                uint32_t written = _client.write(messagePtr, messageSize);
                messagePtr += written;
                messageSize -= written;
                if (messageSize > 0) delay(0);
            }
        }
    }

    void TcpClientHandle::stop()
    {
        _client.stop();
    }

    void TcpClientHandle::flush()
    {
        _client.flush();
    }

    bool TcpClientHandle::isConnected()
    {
        return _client.connected();
    }

    bool TcpClientHandle::isWriteTimeout()
    {
        return _writeTimeout;
    }

    void TcpClientHandle::setStreamId(uint64_t id)
    {
        _streamId = id;
    }

    uint64_t TcpClientHandle::getStreamId() const
    {
        return _streamId;
    }

    WiFiClient& TcpClientHandle::getWiFiClient()
    {
        return _client;
    }

    IPAddress TcpClientHandle::getRemoteIp()
    {
        return _client.remoteIP();
    }
}