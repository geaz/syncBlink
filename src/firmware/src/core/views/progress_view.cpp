#ifndef PROGRESSVIEW_H
#define PROGRESSVIEW_H

#include <display.hpp>
#include <view.hpp>

namespace SyncBlink
{
    class ProgressView : public View
    {
    public:
        ProgressView(const std::string text) : _text(text)
        {
        }

        void drawFrame(DisplayCtrl& display) override
        {
            display.setFontPosCenter();
            uint8_t displayCenter = (display.getDisplayHeight() - 10) / 2;

            display.setFont(u8g2_font_5x8_tf);
            display.drawStr(10, displayCenter - 5, _text.c_str());

            drawProgress(display, 10, displayCenter + 5, display.getDisplayWidth() - 20, 5, _progress);
            display.setFontPosBaseline();
        }

        void setText(const std::string& text)
        {
            _text = text;
        }
        void setProgress(const float progress)
        {
            _progress = progress;
        }

    private:
        void drawProgress(DisplayCtrl& display, int x, int y, int w, int h, float progress)
        {
            display.drawFrame(x, y, w, h);
            display.drawBox(x + 2, y + 2, (w - 4) * progress, h - 3);
        }

        std::string _text;
        float _progress = 0;
    };
}

#endif // PROGRESSVIEW_H