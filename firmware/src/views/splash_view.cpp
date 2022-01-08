#ifndef SPLASHVIEW_H
#define SPLASHVIEW_H

#include <view.hpp>
#include <display.hpp>

namespace SyncBlink
{
    class SplashView : public View
    {
        public:
            void drawFrame(DisplayCtrl& display) override
            {
                display.setFont(u8g2_font_tenfatguys_tr);
                drawVHCenteredStr(display, "syncBlink");
            }
    };
}

#endif // SPLASHVIEW_H