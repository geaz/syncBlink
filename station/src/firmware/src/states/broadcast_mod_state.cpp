#ifndef BROADCASTMODSTATE_H
#define BROADCASTMODSTATE_H

#include <blinkscript/blink_script.hpp>
#include "run_mod_state.cpp"
#include "invalid_mod_state.cpp"
#include "state.hpp"
#include "station_context.hpp"
#include "views/icon_text_view.cpp"

namespace SyncBlink
{
    class BroadcastModState : public State
    {
        public:
            BroadcastModState(StationContext& context, std::string mod)
                : _context(context), _mod(mod)
            {
                _handleId = context.getSocketServer()
                    .messageEvents
                    .addEventHandler([this](SocketMessage message) 
                    { 
                        if(message.messageType == Client::MOD_DISTRIBUTED && _broadcastStartedAt != 0) {
                            
                            _modDistributed = ++_receivedAnswers == _nodeCount;
                        }
                        _resetDistribution = message.messageType == Client::MESH_CONNECTION || message.messageType == Client::MESH_DISCONNECTION;
                    });
                _broadcastModView = std::make_shared<IconTextView>("Broadcasting MOD ...", u8g2_font_open_iconic_thing_2x_t, 74);
            }

            ~BroadcastModState()
            {
                _context.getSocketServer()
                    .messageEvents
                    .removeEventHandler(_handleId);
            }

            void run(StationContext& context)
            {                         
                context.getLed().setAllLeds(SyncBlink::Yellow);
                context.getDisplay().setView(_broadcastModView);
                context.getDisplay().loop();

                if(_resetDistribution)
                {
                    _receivedAnswers = 0;
                    _broadcastStartedAt = 0;
                }
                else if(_broadcastStartedAt == 0 && context.getSocketServer().getClientsCount() > 0)
                {
                    _broadcastStartedAt = millis();
                    _nodeCount = context.getNodeManager().getTotalNodeCount() - 1; // Don't count station node
                    context.getSocketServer().broadcast((void*)_mod.c_str(), _mod.size(), Server::DISTRIBUTE_MOD);
                }
                else if(context.getSocketServer().getClientsCount() == 0) _modDistributed = true;
                else if(_broadcastStartedAt + 10000 < millis()) _modDistributed = true; // Timeout

                if(_modDistributed)
                {
                    auto blinkScript = std::make_shared<BlinkScript>(context.getLed(), _mod);
                    blinkScript->updateLedInfo(0, 0, context.getNodeManager().getTotalLedCount());
                    blinkScript->init();

                    Server::SourceMessage sourceMessage = { context.getModManager().getActiveSource() };
                    context.getSocketServer().broadcast(&sourceMessage, sizeof(sourceMessage), Server::SOURCE_UPDATE);

                    context.getLed().setAllLeds(SyncBlink::Black);
                    context.currentState = std::make_shared<RunModState>(context, blinkScript);
                }
            }

        private:
            StationContext& _context;
            std::string _mod;
            uint64_t _handleId;

            std::shared_ptr<IconTextView> _broadcastModView;
            uint8_t _receivedAnswers = 0;
            uint8_t _nodeCount = 0;
            unsigned long _broadcastStartedAt = 0;
            bool _modDistributed = false;
            bool _resetDistribution = false;
    };
}

#endif // BROADCASTMODSTATE_H