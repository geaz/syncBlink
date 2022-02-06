#ifndef SCRIPTCHANGEMSG_H
#define SCRIPTCHANGEMSG_H

#include "core/script.hpp"
#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

namespace SyncBlink
{
    namespace Messages
    {
        struct ScriptChange : public Message
        {
            ScriptChange() {}
            ScriptChange(Script _script) : script{_script} {}
            MessageType getMessageType() const { return MessageType::ScriptChange; }

            Script script;
        };
    }
}

#endif // SCRIPTCHANGEMSG_H