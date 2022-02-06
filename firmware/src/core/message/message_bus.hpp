#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <functional>
#include <memory>
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

    class MessageBus
    {
    public:
        template <typename MsgType> void trigger(const MsgType message)
        {
            const auto typeIndex = std::type_index(typeid(MsgType));

            const auto range = _registrations.equal_range(typeIndex);
            for (auto it = range.first; it != range.second; it++)
            {
                auto fun = (MessageHandler<MsgType>*)it->second.handler();
                fun->onMsg(message);
            }
        }

        template <typename MsgType> uint32_t addMsgHandler(MessageHandler<MsgType>* handler)
        {
            uint32_t id = _nextId++;
            const auto typeIndex = std::type_index(typeid(MsgType));

            MsgRegistration msgRegistration(id, handler);
            _registrations.emplace(typeIndex, msgRegistration);

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

    private:
        uint32_t _nextId = 0;
        std::unordered_multimap<std::type_index, MsgRegistration> _registrations;
    };
}

#endif // EVENTBUS_H