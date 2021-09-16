#ifndef SENDFIRMWARESTATE_H
#define SENDFIRMWARESTATE_H

#include "state.hpp"
#include "station_context.hpp"
#include "display/views/progress_view.cpp"

#include <LittleFS.h>

namespace SyncBlink
{
    class SendFirmwareState : public State
    {
        public:
            SendFirmwareState(StationContext& context, uint64_t targetId) :
                _progressView(std::make_shared<ProgressView>("Sending Firmware..."))
            {
                _targetId = targetId;
                if(LittleFS.exists(FirmwarePath.c_str()))
                {
                    _activeFlash = LittleFS.open(FirmwarePath.c_str(), "r");
                    _flashPos = 0;
                    _flashing = true;
                    context.getBlinkTcpServer().broadcast(&_targetId, sizeof(_targetId), Server::FIRMWARE_FLASH_START);
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
                        context.getBlinkTcpServer().broadcast(&_targetId, sizeof(_targetId), Server::FIRMWARE_FLASH_END);
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

                        context.getBlinkTcpServer().broadcast(buf, readSize, Server::FIRMWARE_FLASH_DATA);
                        _progressView->setProgress((float)_flashPos/_activeFlash.size());
                    }
                }
            }

        private:
            std::shared_ptr<ProgressView> _progressView;
            File _activeFlash;
            size_t _flashPos;
            bool _flashing;
            uint64_t _targetId;
    };
}

#endif // SENDFIRMWARESTATE_H