#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <memory>
#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <typeindex>

namespace SyncBlink
{
    template<class T>
    class EventHandler
    {
        public:
            virtual void onEvent(const T&) = 0;
    };

    class EventRegistration
    {
    public:
        EventRegistration(uint32_t id, void* handler) : _id(id), _handler(handler) {}

        void* handler() { return _handler; }
        uint32_t id() { return _id; }

    private:
        uint32_t _id;
        void* _handler;
    };

    class EventBus
    {
    public:
        template<typename EventType> void trigger(const EventType eventMessage)
        {
            const auto typeIndex = std::type_index(typeid(EventType));
            _events[typeIndex] = std::shared_ptr<const void>(&eventMessage);

            const auto range = _registrations.equal_range(typeIndex);
            for(auto it = range.first; it != range.second; it++)
            {
                auto fun = (EventHandler<EventType>*) it->second.handler();
                fun->onEvent(eventMessage);
            }
        }

        template<typename EventType> uint32_t addEventHandler(EventHandler<EventType>* handler, bool replayLast = false)
        {
            uint32_t id = _nextId++;
            const auto typeIndex = std::type_index(typeid(EventType));
            
            EventRegistration eventRegistration(id, handler);
            _registrations.emplace(typeIndex, eventRegistration);

            if(replayLast && _events.find(typeIndex) != _events.end())
            {
                handler->onEvent(*((EventType*) _events[typeIndex].get()));
            }

            return eventRegistration.id();
        }

        void removeEventHandler(uint32_t id)
        {
            for(auto it = _registrations.begin(); it != _registrations.end(); it++)
            {
                if(it->second.id() == id)
                {
                    _registrations.erase(it);
                    break;
                }
            }
        }

    private:
        uint32_t _nextId = 0;
        std::unordered_multimap<std::type_index, EventRegistration> _registrations;
        std::unordered_map<std::type_index, std::shared_ptr<const void>> _events;
    };
}

#endif // EVENTBUS_H