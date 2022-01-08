#ifndef BROADCASTSCRIPTSTATE_H
#define BROADCASTSCRIPTSTATE_H

#include "state.hpp"
#include "station_context.hpp"
#include "run_script_state.cpp"
#include "invalid_script_state.cpp"
#include "views/icon_text_view.cpp"

#include <blinkscript/blink_script.hpp>

namespace SyncBlink
{
    class BroadcastScriptState : public State
    {
        public:
            BroadcastScriptState(StationContext& context) : _context(context)
            {
                _handleId = context.getTcpServer()
                    .serverMessageEvents
                    .addEventHandler([this](Message message)
                    { 
                        if(message.type == Client::SCRIPT_DISTRIBUTED && _broadcastStartedAt != 0)
                        {
                            _scriptDistributed = ++_receivedAnswers == _nodeCount;
                        }
                        _resetDistribution = message.type == Client::MESH_CONNECTION || message.type == Client::MESH_DISCONNECTION;
                    });
                _broadcastScriptView = std::make_shared<IconTextView>("Broadcasting script ...", u8g2_font_open_iconic_thing_2x_t, 74);
            }

            ~BroadcastScriptState()
            {
                _context.getTcpServer()
                    .serverMessageEvents
                    .removeEventHandler(_handleId);
            }

            void run(StationContext& context)
            {                         
                context.getLed().showNow(SyncBlink::Yellow);
                context.getDisplay().setView(_broadcastScriptView);
                context.getDisplay().loop();

                readCurrentScript(context);
                
                if(_resetDistribution)
                {
                    _receivedAnswers = 0;
                    _broadcastStartedAt = 0;
                }
                else if(_broadcastStartedAt == 0 && context.getTcpServer().getClientsCount() > 0)
                {                    
                    if(_activeScript.Exists)
                    {
                        _broadcastStartedAt = millis();
                        _nodeCount = context.getNodeManager().getTotalNodeCount() - 1; // Don't count station node
                        context.getTcpServer().broadcast(
                            (void*)_activeScript.Content.c_str(),
                            _activeScript.Content.size(),
                            Server::DISTRIBUTE_SCRIPT);
                    }
                    else context.currentState = std::make_shared<InvalidScriptState>(context);
                }
                else if(context.getTcpServer().getClientsCount() == 0) _scriptDistributed = true;
                else if(_broadcastStartedAt + 10000 < millis()) _scriptDistributed = true; // Timeout

                if(_scriptDistributed)
                {
                    auto blinkScript = std::make_shared<BlinkScript>(context.getLed(), _activeScript.Content);
                    blinkScript->updateLedInfo(0, 0, context.getNodeManager().getTotalLedCount());
                    blinkScript->init();

                    context.getLed().setAllLeds(SyncBlink::Black);
                    context.currentState = std::make_shared<RunScriptState>(context, blinkScript);
                }
            }

        private:
            void readCurrentScript(StationContext& context)
            {
                std::string activescriptName = context.getScriptManager().getActiveScript();
                _activeScript = context.getScriptManager().get(activescriptName);
                _scriptDistributed = true;
            }

            uint32_t _handleId;
            uint32_t _nodeCount;
            StationContext& _context;

            uint8_t _receivedAnswers = 0;
            unsigned long _broadcastStartedAt = 0;

            Script _activeScript;
            bool _scriptRead = false;

            bool _scriptDistributed = false;
            bool _resetDistribution = false;

            std::shared_ptr<IconTextView> _broadcastScriptView;
    };
}

#endif // BROADCASTSCRIPTSTATE_H