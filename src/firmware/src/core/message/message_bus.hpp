#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include "message.hpp"
#include "messages/analyzer_change.hpp"
#include "messages/analyzer_update.hpp"
#include "messages/mesh_connection.hpp"
#include "messages/mesh_update.hpp"
#include "messages/node_command.hpp"
#include "messages/script_change.hpp"
#include "messages/script_error.hpp"

#include <functional>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace SyncBlink
{
    template <class T> class MessageHandler
    {
    public:
        virtual void onMsg(const T&) = 0;
    };

    class MsgRegistration
    {
    public:
        MsgRegistration(uint32_t id, void* handler) : _id(id), _handler(handler)
        {
        }

        void* handler()
        {
            return _handler;
        }

        uint32_t id()
        {
            return _id;
        }

    private:
        uint32_t _id;
        void* _handler;
    };

    /* 
    This class was completly template based by using typeIds. Type IDs require RTTI.
    But removing the build flag "no-rtti" from the build, results in 6KB RAM usage!!!
    Thats why I am using this verbose MessageBus declaration. Open for better ideas! :)
    */
    class MessageBus
    {
    public:
        void trigger(const Messages::AnalyzerChange message)
        {
            trigger(MessageType::AnalyzerChange, message);
        }

        void trigger(const Messages::AnalyzerUpdate message)
        {
            trigger(MessageType::AnalyzerUpdate, message);
        }

        void trigger(const Messages::MeshConnection message)
        {
            trigger(MessageType::MeshConnection, message);
        }

        void trigger(const Messages::MeshUpdate message)
        {
            trigger(MessageType::MeshUpdate, message);
        }

        void trigger(const Messages::NodeCommand message)
        {
            trigger(MessageType::NodeCommand, message);
        }

        void trigger(const Messages::ScriptChange message)
        {
            trigger(MessageType::ScriptChange, message);
        }

        void trigger(const Messages::ScriptError message)
        {
            trigger(MessageType::ScriptError, message);
        }

        template <typename T> uint32_t addMsgHandler(MessageType type, MessageHandler<T>* handler)
        {
            uint32_t id = _nextId++;
            MsgRegistration msgRegistration(id, handler);
            _registrations.emplace(type, msgRegistration);

            return msgRegistration.id();
        }

        void removeMsgHandler(uint32_t id)
        {
            for (auto it = _registrations.begin(); it != _registrations.end(); it++)
            {
                if (it->second.id() == id)
                {
                    _registrations.erase(it);
                    break;
                }
            }
        }

        static void packageToBus(MessageBus& bus, MessagePackage& package)
        {
            switch (package.type)
            {
            case MessageType::AnalyzerChange: {
                Messages::AnalyzerChange message;
                message.loadPackage(package);
                bus.trigger(message);
                break;
            }
            case MessageType::AnalyzerUpdate: {
                Messages::AnalyzerUpdate message;
                message.loadPackage(package);
                bus.trigger(message);
                break;
            }
            case MessageType::MeshConnection: {
                Messages::MeshConnection message;
                message.loadPackage(package);
                bus.trigger(message);
                break;
            }
            case MessageType::MeshUpdate: {
                Messages::MeshUpdate message;
                message.loadPackage(package);
                bus.trigger(message);
                break;
            }
            case MessageType::NodeCommand: {
                Messages::NodeCommand message;
                message.loadPackage(package);
                bus.trigger(message);
                break;
            }
            case MessageType::ScriptChange: {
                Messages::ScriptChange message;
                message.loadPackage(package);
                bus.trigger(message);
                break;
            }
            }
        }

    private:
        template <typename T> void trigger(MessageType type, const T message)
        {
            const auto range = _registrations.equal_range(type);

            for (auto it = range.first; it != range.second; it++)
            {
                auto fun = (MessageHandler<T>*)it->second.handler();
                fun->onMsg(message);
            }
        }

    private:
        uint32_t _nextId = 0;
        std::unordered_multimap<int, MsgRegistration> _registrations;
    };
}

#endif // MESSAGEBUS_H