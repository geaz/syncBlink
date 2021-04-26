#include "led.hpp"

namespace SyncBlink
{
    void LED::setup(uint32_t ledCount)
    {
        _ledCount = ledCount;
        _leds.resize(_ledCount);
        _groups.resize(_ledCount);
        
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

    void LED::setAllLeds(const Color color)
    {
        for (uint32_t index = 0; index < _leds.size(); index++)
            _leds[index] = color;
    }

    void LED::setLed(const uint32_t index, const Color color)
    {
        if (index >= 0 && index <= _ledCount - 1)
        {
            if (_groupsSet)
            {
                for (uint32_t i : _groups[index])
                {
                    if (i >= 0 && i <= _ledCount - 1)
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
        if (index >= 0 && index <= _ledCount - 1)
        {
            if (_groupsSet && _groups[index].size() > 0)
                color = _leds[_groups[index].front()];
            else
                color = _leds[index];
        }
        return (color.r << 16) | (color.g << 8) | color.b;
    }

    uint32_t LED::getLedCount() const
    {
        return _ledCount;
    }

    void LED::setGroup(const uint32_t index, std::vector<uint32_t> group)
    {
        if (index >= 0 && index <= _ledCount - 1)
        {
            _groups[index] = group;
            _groupsSet = true;
        }
    }

    void LED::clearGroups()
    {
        for (uint32_t i = 0; i < _ledCount; i++)
            _groups[i].clear();
        _groupsSet = false;
    }

    void LED::blinkNow(const Color color)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            setAllLeds(color);
            loop();
            delay(250);
            setAllLeds(Black);
            loop();
            delay(250);
        }        
    }
}