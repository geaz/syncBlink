#ifndef SERIALAPI_H
#define SERIALAPI_H

#include "node_manager.hpp"
#include <messages/api_messages.hpp>

namespace SyncBlink
{
    class SerialApi
    {
        public:
            SerialApi(NodeManager& nodeManager);
            void loop();

        private:
            NodeManager& _nodeManager;
    };
}

#endif // SERIALAPI_H