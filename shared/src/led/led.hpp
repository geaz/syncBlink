#ifndef LED_H
#define LED_H

#define FASTLED_INTERNAL
#include "color.hpp"

#include <FastLED.h>
#include <array>
#include <memory>
#include <vector>

namespace SyncBlink
{
    class LED
    {
    public:
        LED();

        void loop();
        void setLed(const uint32_t index, const Color color);
        uint32_t getLed(const uint32_t index);

        void setAllLeds(const Color color);

        void setGroup(const uint32_t index, std::vector<uint32_t> group);
        void clearGroups();

    private:

        bool _groupsSet = false;
        std::array<CRGB, LED_COUNT> _leds;
        std::array<std::vector<uint32_t>, LED_COUNT> _groups;
    };
}

#endif // LED_H