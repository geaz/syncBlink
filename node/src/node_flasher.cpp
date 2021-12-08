#include "node_flasher.hpp"

namespace SyncBlink
{
    NodeFlasher::NodeFlasher(TcpClient& tcpClient, TcpServer& tcpServer, LED& led)
    {
        tcpClient
            .firmwareFlashEvents
            .addEventHandler([this, &tcpServer, &led](std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType) 
                { onFirmwareFlashReceived(data, targetNodeId, messageType, tcpServer, led); }
            );
    }

    void NodeFlasher::onFirmwareFlashReceived(std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType, TcpServer& tcpServer, LED& led)
    {
        bool restart = false;
        if(targetNodeId == SyncBlink::getId() || targetNodeId == 0)
        {
            if(messageType == Server::FIRMWARE_FLASH_START)
            {
                led.blinkNow(Yellow);
                uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                if (!Update.begin(maxSketchSpace))
                {
                    led.blinkNow(Red);
                    Update.printError(Serial);
                }
                else
                {
                    led.showNow(Blue);
                    _flashActive = true;
                }
            }
            else if(messageType == Server::FIRMWARE_FLASH_END)
            {
                if (Update.end(true))
                {
                    led.blinkNow(Cyan);                    
                    _flashActive = false;
                    Serial.println("Update Success: Rebooting...\n");
                    restart = true;
                }
                else
                {
                    led.blinkNow(Red);
                    Update.printError(Serial);
                }
            }
        }   
        
        // NodeId for FLash Data is '0' - that why we handle it in a seperate if branch
        if(_flashActive && messageType == Server::FIRMWARE_FLASH_DATA)
        {            
            if (Update.write(&data[0], data.size()) != data.size())
            {
                led.blinkNow(Red);
                Update.printError(Serial);
            }
        }
        
        // Only distribute, if the message is not for the current node
        // Or all nodes in the mesh get flashed
        if(!_flashActive ||  targetNodeId == 0) tcpServer.broadcast(&data[0], data.size(), messageType);

        // we are restarting here to make sure, that all messages 
        // also get send to the current child nodes
        if(restart)
        {
            tcpServer.loop();
            led.showNow(Black);
            ESP.restart();
        }
    }
}