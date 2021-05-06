#ifndef EVENTREGISTRATION_H
#define EVENTREGISTRATION_H

#include <map>
#include <Arduino.h>

namespace SyncBlink
{
    template <class T> class EventRegistration
    {
    public:
        uint32_t addEventHandler(const T event)
        {
            uint32_t id = _nextId++;
            _events[id] = event;
            return id;
        }

        void removeEventHandler(const uint32_t id)
        {
            _events.erase(id);
        }

        std::map<uint32_t, T> getEventHandlers() const
        {
            return _events;
        }

    private:
        uint32_t _nextId = 0;
        std::map<uint32_t, T> _events;
    };
}

#endif // EVENTREGISTRATION_H