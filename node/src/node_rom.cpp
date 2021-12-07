#include "node_rom.hpp"

#include <Arduino.h>
#include <shared_constants.hpp>
#include <rom_funcs.hpp>

namespace SyncBlink
{
    void NodeRom::setup(TcpClient& tcpClient, TcpServer& tcpServer)
    {
        tcpClient
            .nodeRenameEvents
            .addEventHandler([this, &tcpServer](Server::NodeRenameMessage message) { onNodeRenameReceived(message, tcpServer); });
        tcpClient
            .nodeSetWifiEvents
            .addEventHandler([this, &tcpServer](Server::WifiSetMessage message) { onSetWifiReceived(message, tcpServer); });
    }

    uint32_t NodeRom::getLedCount() { readRom(); return _nodeLedCount; }
    std::string NodeRom::getNodeLabel() { readRom(); return _nodeLabel; }
    std::string NodeRom::getSSID() { readRom(); return _ssid; }
    std::string NodeRom::getWifiPw() { readRom(); return _wifiPw; }
        
    void NodeRom::readRom()
    {
        if(!_romRead)
        {
            _nodeLedCount = SyncBlink::readRomInt8(0);
            Serial.printf("EEPROM LED Count: %i\n", _nodeLedCount);

            _ssid = SyncBlink::readRomString(SyncBlink::WifiRomSSIDStart, SyncBlink::WifiRomSSIDLength);
            Serial.printf("EEPROM SSID: %s\n", _ssid.c_str());

            _wifiPw = SyncBlink::readRomString(SyncBlink::WifiRomPwStart, SyncBlink::WifiRomPwLength);
            Serial.printf("EEPROM WiFi PW: %s\n", _wifiPw.c_str());

            _nodeLabel = SyncBlink::readRomString(SyncBlink::LabelRomStart, SyncBlink::MaxNodeLabelLength);
            Serial.printf("EEPROM Node Label: %s\n", _nodeLabel.c_str());

            _romRead = true;
        }        
    }

    void NodeRom::onNodeRenameReceived(Server::NodeRenameMessage message, TcpServer& tcpServer)
    {
        if(message.nodeId == SyncBlink::getId())
        {
            SyncBlink::writeRomString(SyncBlink::LabelRomStart, &message.nodeLabel[0], MaxNodeLabelLength);
            _romRead = false;
        }
        else tcpServer.broadcast(&message, sizeof(message), Server::NODE_RENAME);
    }

    void NodeRom::onSetWifiReceived(Server::WifiSetMessage message, TcpServer& tcpServer)
    {
        if(message.nodeId == SyncBlink::getId())
        {
            Serial.println("drin");
            Serial.println(message.ssid);
            Serial.println(message.pw);

            SyncBlink::writeRomString(SyncBlink::WifiRomSSIDStart, &message.ssid[0], WifiRomSSIDLength);
            SyncBlink::writeRomString(SyncBlink::WifiRomPwStart, &message.pw[0], WifiRomPwLength);
            ESP.restart();
        }
        else tcpServer.broadcast(&message, sizeof(message), Server::SET_WIFI);
    }
}