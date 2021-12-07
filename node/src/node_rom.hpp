#ifndef NODEROM_H
#define NODEROM_H

#include <network/mesh/syncblink_mesh.hpp>
#include <network/tcp/tcp_server.hpp>
#include <string>
#include <stdint.h>

namespace SyncBlink
{
    const uint8_t LabelRomStart = 100;
    const uint8_t LedCountRomPos = 0;

    const uint8_t WifiRomSSIDStart = 4;
    const uint8_t WifiRomSSIDEnd = 36;
    const uint8_t WifiRomSSIDLength = WifiRomSSIDEnd - WifiRomSSIDStart;

    const uint8_t WifiRomPwStart = 36;
    const uint8_t WifiRomPwEnd = 100;
    const uint8_t WifiRomPwLength = WifiRomPwEnd - WifiRomPwStart;
    
    class NodeRom
    {
    public:
        void setup(TcpClient& tcpClient, TcpServer& tcpServer);

        uint32_t getLedCount();
        std::string getNodeLabel();
        std::string getSSID();
        std::string getWifiPw();
    
    private:
        void readRom();
        void onNodeRenameReceived(Server::NodeRenameMessage message, TcpServer& tcpServer);

        bool _romRead = false;
        uint32_t _nodeLedCount = 0;
        std::string _nodeLabel;
        std::string _ssid;
        std::string _wifiPw;
    };
}

#endif // NODEROM_H