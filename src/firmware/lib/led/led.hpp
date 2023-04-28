#ifndef LED_H
#define LED_H

#define FASTLED_INTERNAL
#include "color.hpp"

#include <Arduino.h>
#include <FastLED.h>
#include <vector>

namespace SyncBlink
{
    class LED
    {
    public:
        void setup(uint16_t ledCount);
        void loop();

        void setAllLeds(const Color color);

        void setLed(const uint16_t index, const Color color);
        uint16_t getLed(const uint16_t index);
        uint16_t getLedCount() const;

        void setGroup(const uint16_t index, std::vector<uint16_t> group);
        void clearGroups();

        void showNow(const Color color);
        void blinkNow(const Color color, uint8_t times = 3);

    private:
        uint32_t _ledCount = 0;
        bool _groupsSet = false;

        std::vector<CRGB> _leds;
        std::vector<std::vector<uint16_t>> _groups;
    };
}

#endif // LED_H