#ifndef BROADCASTSCRIPTSTATE_H
#define BROADCASTSCRIPTSTATE_H

#include "state.hpp"
#include "run_script_state.cpp"
#include "invalid_script_state.cpp"
#include "views/icon_text_view.cpp"
#include "scripts/script_manager.hpp"

#include <event_bus.hpp>
#include <blink_script.hpp>
#include <tcp/tcp_server.hpp>


namespace SyncBlink
{
    class BroadcastScriptState : public State, public EventHandler<ScriptChangedEvent>, public EventHandler<ClientEvent>
    {
        public:
            BroadcastScriptState(EventBus& eventBus, LED& led, Display& display, Script activeScript, uint32_t clientCount) :
                _eventBus(eventBus),
                _led(led),
                _display(display),
                _activeScript(activeScript),
                _clientCount(clientCount)
            {
                _clientHandleId = _eventBus.addEventHandler<ClientEvent>(this);
                _scriptHandleId = _eventBus.addEventHandler<ScriptChangedEvent>(this);
                _broadcastScriptView = std::make_shared<IconTextView>("Broadcasting script ...", u8g2_font_open_iconic_thing_2x_t, 74);
            }

            ~BroadcastScriptState()
            {
                _eventBus.removeEventHandler(_clientHandleId);
                _eventBus.removeEventHandler(_scriptHandleId);
            }

            void loop()
            {                         
                _led.showNow(SyncBlink::Yellow);
                _display.setView(_broadcastScriptView);
                _display.loop();
                
                if(_resetDistribution)
                {
                    _receivedAnswers = 0;
                    _broadcastStartedAt = 0;
                    _scriptDistributed = false;
                    _resetDistribution = false;
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

            void onEvent(const ClientEvent& event)
            {
                if(event.MessageType == Client::SCRIPT_DISTRIBUTED && _broadcastStartedAt != 0)
                {
                    _scriptDistributed = ++_receivedAnswers == _nodeCount;
                }
                _resetDistribution = event.MessageType == Client::MESH_CONNECTION || event.MessageType == Client::MESH_DISCONNECTION;
                _clientCount = event.TotalClients;
            }

            void onEvent(const ScriptChangedEvent& event)
            {
                _activeScript = event.script;
                _resetDistribution = true;
            }

        private:
            EventBus& _eventBus;
            LED& _led;
            Display& _display;

            Script _activeScript;
            uint32_t _clientCount;

            uint32_t _scriptHandleId;
            uint32_t _clientHandleId;





            uint8_t _receivedAnswers = 0;
            unsigned long _broadcastStartedAt = 0;


            bool _scriptDistributed = false;
            bool _resetDistribution = false;

            std::shared_ptr<IconTextView> _broadcastScriptView;
    };
}

#endif // BROADCASTSCRIPTSTATE_H