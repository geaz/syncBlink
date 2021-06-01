#include "tcp_stream.hpp"
#include "shared_constants.hpp"

namespace SyncBlink
{
    TcpStream::TcpStream()
    {
        _client.setNoDelay(true);
    }

    TcpStream::TcpStream(WiFiClient client) : _client(client)
    {
        _client.setNoDelay(true);
        _remoteIp = _client.remoteIP();
    }

    void TcpStream::stop()
    {
        _client.stop();
    }

    void TcpStream::flush()
    {
        _client.flush();
    }

    bool TcpStream::connectTo(String socketIp, uint16_t port)
    {
        bool connected = false;
        #ifdef DEBUG_TCPSTREAM
        Serial.println("[TCP STREAM] Connecting to TCP '" + socketIp + "' ...");
        #endif
        if(_client.connect(socketIp, 81)){
            #ifdef DEBUG_TCPSTREAM
            Serial.println("[TCP STREAM] Connected!");
            #endif
            connected = true;
        }
        return connected;
    }

    bool TcpStream::checkMessage(TcpMessage& tcpMessage)
    {
        bool receivedMessage = false;
        while(_client.available())
        {
            uint8_t magicBuf[2];
            uint8_t byte = _client.read();
            if(byte == SocketMagicBytes[0])
            {
                _client.read(&magicBuf[0], 2);
                if(magicBuf[0] == SocketMagicBytes[1] && magicBuf[1] == SocketMagicBytes[2])
                {
                    uint32_t messageSize = (_client.read()<<24) + 
                        (_client.read()<<16) + 
                        (_client.read()<<8) + 
                        _client.read();
                    uint8_t messageChecksum = _client.read();
                    uint8_t messageType = _client.read();
                    
                    uint8_t checksum = 0;
                    checksum += SocketMagicBytes[0] % 2;                    
                    checksum += SocketMagicBytes[1] % 2;
                    checksum += SocketMagicBytes[2] % 2;
                    checksum += (uint8_t)(messageSize>>24) % 2;
                    checksum += (uint8_t)(messageSize>>16) % 2;
                    checksum += (uint8_t)(messageSize>>8) % 2;
                    checksum += (uint8_t)(messageSize>>0) % 2;

                    if(checksum == messageChecksum)
                    {
                        tcpMessage.messageType = messageType;
                        tcpMessage.message.resize(messageSize);
                        uint32_t readBytes = 0;
                        while(readBytes < messageSize)
                        {
                            readBytes += _client.read(&tcpMessage.message[readBytes], messageSize-readBytes);
                            if(readBytes != messageSize)
                            {
                                #ifdef DEBUG_TCPSTREAM
                                Serial.printf("[TCP STREAM] Big message, have to yield...\n");
                                #endif
                                delay(0);
                            }
                        }
                        receivedMessage = true; 
                          
                        #ifdef DEBUG_TCPSTREAM
                        Serial.printf("[TCP STREAM] Found message - Size: %i, Type: %i (%i)\n", tcpMessage.message.size() + SocketHeaderSize, tcpMessage.messageType, receivedMessage);
                        #endif

                        break;
                    }
                }
            }
        }
        return receivedMessage;
    }

    void TcpStream::writeMessage(std::vector<uint8_t> message)
    {
        if(_client.connected())
        {
            long started = millis();
            uint8_t* messagePtr = &message[0];
            uint32_t messageSize = message.size();
            while(messageSize > 0)
            {
                if(millis() - started > SocketWriteTimeout)
                {
                    #ifdef DEBUG_TCPSTREAM
                    Serial.printf("[TCP SERVER] Write Timeout Client\n");
                    #endif
                    _writeTimeout = true;
                    break;
                }
                
                uint32_t written = _client.write(messagePtr, messageSize);
                messagePtr += written;
                messageSize -= written;
                if(messageSize > 0) delay(0);
            }
        }
    }

    bool TcpStream::isWriteTimeout()
    {
        return _writeTimeout;
    }

    bool TcpStream::isConnected()
    {
        return _client.connected();
    }

    void TcpStream::setStreamId(uint64_t id)
    {
        _streamId = id;
    }

    uint64_t TcpStream::getStreamId() const
    {
        return _streamId;
    }

    IPAddress TcpStream::getRemoteIp() const
    {
        return _remoteIp;
    }

    std::vector<uint8_t> TcpStream::serializeMessage(void* message, uint32_t messageSize, uint8_t messageType)
    {
        uint32_t packageSize = messageSize + SocketHeaderSize;
        
        std::vector<uint8_t> package;
        package.resize(packageSize);

        package[0] = SocketMagicBytes[0];
        package[1] = SocketMagicBytes[1];
        package[2] = SocketMagicBytes[2];
        package[3] = (uint8_t)(messageSize>>24);
        package[4] = (uint8_t)(messageSize>>16);
        package[5] = (uint8_t)(messageSize>>8);
        package[6] = (uint8_t)(messageSize>>0);

        uint8_t checksum = 0;
        for(uint8_t i = 0; i<7; i++)
        {
            checksum += package[i] % 2;
        }
        package[7] = checksum;
        package[8] = messageType;
        if(messageSize > 0) memcpy(&package[9], message, messageSize);

        return package;
    }
}