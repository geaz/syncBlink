#ifndef LED_H
#define LED_H

#define FASTLED_INTERNAL
#include "color.hpp"

#include <FastLED.h>
#include <vector>

namespace SyncBlink
{
    class LED
    {
    public:
        void setup(uint32_t ledCount);
        void loop();

        void setAllLeds(const Color color);

        void setLed(const uint32_t index, const Color color);
        uint32_t getLed(const uint32_t index);
        uint32_t getLedCount() const;

        void setGroup(const uint32_t index, std::vector<uint32_t> group);
        void clearGroups();

        void showNow(const Color color);
        void blinkNow(const Color color);

    private:
        uint32_t _ledCount = 0;
        bool _groupsSet = false;

        std::vector<CRGB> _leds;
        std::vector<std::vector<uint32_t>> _groups;
    };
}

#endif // LED_H