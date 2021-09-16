#ifndef APISERVER_H
#define APISERVER_H

#include "node_manager.hpp"
#include <messages/api_messages.hpp>

namespace SyncBlink
{
    class ApiServer
    {
        public:
            ApiServer(NodeManager& nodeManager);
            void loop();

        private:
            NodeManager& _nodeManager;
    };
}

#endif // APISERVER_H