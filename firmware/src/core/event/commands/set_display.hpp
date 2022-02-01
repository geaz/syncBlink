#ifndef SETDISPLAYCOMMAND_H
#define SETDISPLAYCOMMAND_H

#include <memory>
#include <string>
#include <view.hpp>

namespace SyncBlink
{
    namespace Commands
    {
        struct SetDisplay
        {
            std::shared_ptr<View> view;
            std::string leftStatus;
        };
    }
}

#endif // SETDISPLAYCOMMAND_H