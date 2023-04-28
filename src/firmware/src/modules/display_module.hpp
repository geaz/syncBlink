#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "core/message/commands/set_display.hpp"
#include "core/message/message_bus.hpp"
#include "module.hpp"

#include <display.hpp>

namespace SyncBlink
{
    class DisplayModule : public Module, public MessageHandler<Commands::SetDisplay>
    {
    public:
        DisplayModule(MessageBus& messageBus);
        ~DisplayModule();

        void setup() override;
        void loop();

        void onMsg(const Commands::SetDisplay& command);

    private:
        Display _display;
        MessageBus& _messageBus;
        uint32_t _displayCommandHandleId = 0;
    };
}

#endif // DISPLAYMODULE_H