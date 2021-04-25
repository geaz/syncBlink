#include "led.hpp"

namespace SyncBlink
{
    LED::LED()
    {
        for (uint32_t index = 0; index < _leds.size(); index++)
            _leds[index] = CRGB::Black;

        FastLED.addLeds<WS2812B, LED_PIN, GRB>(_leds.data(), _leds.size()).setCorrection(TypicalLEDStrip);
        FastLED.setTemperature(Candle);
        FastLED.show();
    }

    void LED::loop()
    {
        FastLED.show();
    }

    void LED::setLed(const uint32_t index, const Color color)
    {
        if (index >= 0 && index <= LED_COUNT - 1)
        {
            if (_groupsSet)
            {
                for (uint32_t i : _groups[index])
                {
                    if (i >= 0 && i <= LED_COUNT - 1)
                        _leds[i] = color;
                }
            }
            else
                _leds[index] = color;
        }
    }

    uint32_t LED::getLed(const uint32_t index)
    {
        CRGB color = CRGB::Black;
        if (index >= 0 && index <= LED_COUNT - 1)
        {
            if (_groupsSet && _groups[index].size() > 0)
                color = _leds[_groups[index].front()];
            else
                color = _leds[index];
        }
        return (color.r << 16) | (color.g << 8) | color.b;
    }

    void LED::setAllLeds(const Color color)
    {
        for (uint32_t index = 0; index < _leds.size(); index++)
            _leds[index] = color;
    }

    void LED::setGroup(const uint32_t index, std::vector<uint32_t> group)
    {
        if (index >= 0 && index <= LED_COUNT - 1)
        {
            _groups[index] = group;
            _groupsSet = true;
        }
    }

    void LED::clearGroups()
    {
        for (uint32_t i = 0; i < LED_COUNT; i++)
            _groups[i].clear();
        _groupsSet = false;
    }
}