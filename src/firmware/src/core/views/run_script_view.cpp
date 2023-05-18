#ifndef RUNSCRIPTVIEW_H
#define RUNSCRIPTVIEW_H

#include "core/audio/analyzer_constants.hpp"
#include "core/audio/map_funcs.hpp"

#include <display.hpp>
#include <stdio.h>
#include <view.hpp>

namespace SyncBlink
{
    class RunScriptView : public View
    {
    public:
        RunScriptView()
        {
            for (uint8_t i = 0; i < BAR_COUNT; i++)
            {
                _freqBars[i] = 0;
            }
        }

        void drawFrame(DisplayCtrl& display) override
        {
            for (uint8_t i = 0; i < _freqBars.size(); i++)
            {
                drawFrequencyBar(display, i, _freqBars[i]);
            }

            display.setFontPosTop();
            display.setFont(u8g2_font_5x8_tf);

            if (!_strPosCalculated)
            {
                uint8_t volStrWidth = display.getStrWidth("-000.0dB") + display.getStrWidth("/000ms ");
                _volStrXPos = display.getDisplayWidth() - volStrWidth;
                _strPosCalculated = true;
            }

            std::array<char, 32> dBDeltaBuf;
            std::array<char, 16> freqBuf;

            std::sprintf(dBDeltaBuf.data(), "%06.1fdB/%03ims", decibel, delta);
            std::sprintf(freqBuf.data(), "%05ihz", dominantFrequency);

            display.drawStr(0, 0, freqBuf.data());
            display.drawStr(_volStrXPos, 0, dBDeltaBuf.data());
            display.drawHLine(0, 10, display.getDisplayWidth());

            display.setFontPosBaseline();
        }

        void fadeFrequencyRange()
        {
            for (uint8_t i = 0; i < _freqBars.size(); i++)
            {
                if (_freqBars[i] > 0) _freqBars[i]--;
            }
        }

        void setFreqBars(const std::array<uint8_t, TotalResultFreqBins>& freqBins)
        {
            auto newFreqBars = SyncBlink::getScaledMovingAverage<uint8_t, uint8_t, TotalResultFreqBins, TotalResultFreqBins-BAR_COUNT>(freqBins, 0, BAR_HEIGHT);
            for (uint8_t i = 0; i < BAR_COUNT; i++)
            {
                if (newFreqBars[i] < _freqBars[i]) _freqBars[i]--;
                else _freqBars[i] = newFreqBars[i];
            }
        }

        uint32_t delta = 0;
        uint8_t volume = 0;
        float decibel = 0.0;
        uint16_t dominantFrequency = 0;

    private:
        void drawFrequencyBar(DisplayCtrl& display, const uint8_t index, const uint8_t height)
        {
            for (uint8_t i = 1; i <= height; i++)
            {
                uint8_t x = index /* gap */ + (index * 2) /* size */;
                uint8_t y = display.getDisplayHeight() - 12 /* bottom start (statusbar) */ - (i * 2) /* gap and height */;
                display.drawBox(x, y, 2, 1);
            }
        }

        uint8_t _volStrXPos = 0;
        uint8_t _freqStrXPos = 0;
        bool _strPosCalculated = false;
        std::array<uint8_t, BAR_COUNT> _freqBars;
    };
}

#endif // RUNSCRIPTVIEW_H