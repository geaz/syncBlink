#ifndef SENDFIRMWARESTATE_H
#define SENDFIRMWARESTATE_H

#include "state.hpp"
#include "station_context.hpp"
#include "views/progress_view.cpp"

#include <LittleFS.h>

namespace SyncBlink
{
    class SendFirmwareState : public State
    {
        public:
            SendFirmwareState(StationContext& context) :
                _progressView(std::make_shared<ProgressView>("Sending Firmware..."))
            {
                if(LittleFS.exists(FirmwarePath.c_str()))
                {
                    _activeFlash = LittleFS.open(FirmwarePath.c_str(), "r");
                    _flashPos = 0;
                    _flashing = true;
                    context.getTcpServer().broadcast(0, 0, Server::FIRMWARE_FLASH_START);
                }
            }

            void run(StationContext& context)
            {                    
                context.getLed().setAllLeds(SyncBlink::Yellow);
                context.getDisplay().setView(_progressView);

                if(_flashing)
                {
                    if(_flashPos == _activeFlash.size())
                    {
                        _activeFlash.close();
                        _flashing = false;
                        context.getTcpServer().broadcast(0, 0, Server::FIRMWARE_FLASH_END);
                        context.resetState();
                    }
                    else
                    {
                        size_t readSize = _activeFlash.size() - _flashPos > 512 
                            ? 512
                            : _activeFlash.size() - _flashPos;

                        char buf[readSize];
                        _activeFlash.readBytes(buf, readSize);
                        _flashPos += readSize;

                        context.getTcpServer().broadcast(buf, readSize, Server::FIRMWARE_FLASH_DATA);
                        _progressView->setProgress((float)_flashPos/_activeFlash.size());
                    }
                }
            }

        private:
            std::shared_ptr<ProgressView> _progressView;
            File _activeFlash;
            size_t _flashPos;
            bool _flashing;
    };
}

#endif // SENDFIRMWARESTATE_H