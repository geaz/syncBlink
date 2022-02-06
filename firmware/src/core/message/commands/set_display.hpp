#ifndef SETDISPLAYCOMMAND_H
#define SETDISPLAYCOMMAND_H

#include "core/message/message.hpp"

#include <memory>
#include <string>
#include <view.hpp>

namespace SyncBlink
{
    namespace Commands
    {
        struct SetDisplay : public Message
        {
            SetDisplay(std::shared_ptr<View> _view, std::string _leftStatus) : view{_view}, leftStatus{_leftStatus} {}

            std::shared_ptr<View> view;
            std::string leftStatus;
        };
    }
}

#endif // SETDISPLAYCOMMAND_H