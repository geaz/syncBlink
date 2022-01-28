#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace SyncBlink
{
    template <class T> class EventHandler
    {
    public:
        virtual void onEvent(const T&) = 0;
    };

    class EventRegistration
    {
    public:
        EventRegistration(uint32_t id, void* handler) : _id(id), _handler(handler)
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

    class EventBus
    {
    public:
        template <typename EventType> void trigger(const EventType eventMessage)
        {
            const auto typeIndex = std::type_index(typeid(EventType));

            const auto range = _registrations.equal_range(typeIndex);
            for (auto it = range.first; it != range.second; it++)
            {
                auto fun = (EventHandler<EventType>*)it->second.handler();
                fun->onEvent(eventMessage);
            }
        }

        template <typename EventType> uint32_t addEventHandler(EventHandler<EventType>* handler)
        {
            uint32_t id = _nextId++;
            const auto typeIndex = std::type_index(typeid(EventType));

            EventRegistration eventRegistration(id, handler);
            _registrations.emplace(typeIndex, eventRegistration);

            return eventRegistration.id();
        }

        void removeEventHandler(uint32_t id)
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
        std::unordered_multimap<std::type_index, EventRegistration> _registrations;
    };
}

#endif // EVENTBUS_H