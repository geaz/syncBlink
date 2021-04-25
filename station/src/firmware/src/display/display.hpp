#ifndef DISPLAY_H
#define DISPLAY_H

#include <memory>
#include <U8g2lib.h>
#include "view.hpp"

namespace SyncBlink
{
    typedef U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI DisplayCtrl;

    class Display
    {
        public:
            Display();

            void init();
            void loop();

            void setView(const std::shared_ptr<View> view);
            void setLeftStatus(const std::string& text);
            void setRightStatus(const std::string& text);

        private:
            DisplayCtrl _ctrl;
            std::shared_ptr<View> _currentView;

            std::string _leftStatusText = "";
            std::string _rightStatusText = "";
    };
}

#endif // DISPLAY_H