#include <catch2/catch.hpp>
#include <iostream>

#include "network/websocket/messages/client_messages.hpp"
#include "network/websocket/messages/server_messages.hpp"

uint32_t _ledCount = 9;
uint32_t _answers = 0;
SyncBlink::Client::MessageType _waitFor = SyncBlink::Client::NONE;
SyncBlink::Client::Message _savedMessage = {0};
SyncBlink::Server::Message _sendMessage = {0};

// Copy of the websocket server method.
// I know - not ideal, but this way is is easy testable on a PC
SyncBlink::Client::Message handleReceivedMessage(SyncBlink::Client::Message receivedMessage, uint32_t connectedClients)
{
    bool forwardMessage = false;
    if(_waitFor == receivedMessage.messageType && (_sendMessage.id == receivedMessage.id || _waitFor == SyncBlink::Client::MOD_DISTRIBUTED))
    {
        _answers++;
        forwardMessage = _answers == connectedClients;

        switch (receivedMessage.messageType)
        {
        case SyncBlink::Client::MESH_COUNTED:
            if (_savedMessage.id == 0)
                _savedMessage = receivedMessage;
            else
            {
                _savedMessage.countedMessage.totalLedCount += receivedMessage.countedMessage.totalLedCount;
                _savedMessage.countedMessage.totalNodeCount += receivedMessage.countedMessage.totalNodeCount;
                if (_savedMessage.countedMessage.routeLedCount < receivedMessage.countedMessage.routeLedCount)
                    _savedMessage.countedMessage.routeLedCount = receivedMessage.countedMessage.routeLedCount;
                if (_savedMessage.countedMessage.routeNodeCount < receivedMessage.countedMessage.routeNodeCount)
                    _savedMessage.countedMessage.routeNodeCount = receivedMessage.countedMessage.routeNodeCount;
            }

            if(forwardMessage) // Last answer received
            {
                _savedMessage.countedMessage.totalNodeCount++;
                _savedMessage.countedMessage.totalLedCount += _ledCount;
                _savedMessage.countedMessage.routeNodeCount++;
                _savedMessage.countedMessage.routeLedCount += _ledCount;
            }
            break;
        default:
            _savedMessage = receivedMessage;
            break;
        }

        if (forwardMessage)
        {
            receivedMessage = _savedMessage;
            _answers = 0;
            _savedMessage.id = 0;
            _waitFor = SyncBlink::Client::NONE;
        }
    }
    else if(receivedMessage.messageType == SyncBlink::Client::EXTERNAL_ANALYZER 
        ||receivedMessage.messageType == SyncBlink::Client::MESH_CONNECTION)
    {
        forwardMessage = true;
    }

    if (forwardMessage) return receivedMessage;
    return {0};
}

TEST_CASE("Websocket Server collects mod distribution successfully", "[Websocket]")
{
    _waitFor = SyncBlink::Client::MOD_DISTRIBUTED;
    auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MOD_DISTRIBUTED }, 1);

    REQUIRE(reveiced.id == 1);
    REQUIRE(reveiced.messageType == SyncBlink::Client::MOD_DISTRIBUTED);
}

TEST_CASE("Websocket Server collects mod distribution for multiple clients successfully", "[Websocket]")
{
    _waitFor = SyncBlink::Client::MOD_DISTRIBUTED;
    for(uint32_t i = 0; i < 5; i++)
    {
        auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MOD_DISTRIBUTED }, 5);
        if(i != 4) REQUIRE(reveiced.id == 0);
        else 
        {
            REQUIRE(reveiced.id == 1);
            REQUIRE(reveiced.messageType == SyncBlink::Client::MOD_DISTRIBUTED);
            REQUIRE(_waitFor == SyncBlink::Client::NONE);
        }
    }
}

TEST_CASE("Websocket Server collects mesh update for multiple clients successfully", "[Websocket]")
{
    _waitFor = SyncBlink::Client::MESH_UPDATED;
    _sendMessage = {1, SyncBlink::Server::MESH_UPDATE };
    for(uint32_t i = 0; i < 5; i++)
    {
        auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MESH_UPDATED }, 5);
        if(i != 4) REQUIRE(reveiced.id == 0);
        else 
        {
            REQUIRE(reveiced.id == 1);
            REQUIRE(reveiced.messageType == SyncBlink::Client::MESH_UPDATED);
            REQUIRE(_waitFor == SyncBlink::Client::NONE);
        }
    }
}

TEST_CASE("Websocket Server should not forward not known collection answer", "[Websocket]")
{
    _waitFor = SyncBlink::Client::MESH_UPDATED;
    _sendMessage = {1, SyncBlink::Server::MESH_UPDATE };

    auto reveiced = handleReceivedMessage({2, SyncBlink::Client::MESH_UPDATED }, 5);
    REQUIRE(reveiced.id == 0);
}

TEST_CASE("Websocket Server collects mesh count successfully", "[Websocket]")
{
    _waitFor = SyncBlink::Client::MESH_COUNTED;
    
    _sendMessage = {1, SyncBlink::Server::MESH_COUNT_REQUEST };
    _sendMessage.countMessage = { 0, 0 };

    SyncBlink::Client::Message receivedClientMessage = {1, SyncBlink::Client::MESH_COUNTED };
    receivedClientMessage.countedMessage = { 16, 1, 16, 1};

    auto reveiced = handleReceivedMessage(receivedClientMessage, 1);
    
    REQUIRE(reveiced.id == 1);
    REQUIRE(reveiced.messageType == SyncBlink::Client::MESH_COUNTED);
    REQUIRE(reveiced.countedMessage.routeLedCount == _ledCount + 16);
    REQUIRE(reveiced.countedMessage.totalLedCount == _ledCount + 16);
    REQUIRE(reveiced.countedMessage.routeNodeCount == 2);
    REQUIRE(reveiced.countedMessage.totalNodeCount == 2);
    REQUIRE(_waitFor == SyncBlink::Client::NONE);
}

TEST_CASE("Websocket Server collects mesh count for multiple clients successfully", "[Websocket]")
{
    _waitFor = SyncBlink::Client::MESH_COUNTED;
    
    _sendMessage = {1, SyncBlink::Server::MESH_COUNT_REQUEST };
    _sendMessage.countMessage = { 0, 0 };

    SyncBlink::Client::Message receivedClientMessage = {1, SyncBlink::Client::MESH_COUNTED };
    for(uint32_t i = 0; i < 5; i++)
    {        
        receivedClientMessage.countedMessage = { 16 + i, 1 + i, 16 + i, 1 + i};
        auto reveiced = handleReceivedMessage(receivedClientMessage, 5);

        if(i != 4) REQUIRE(reveiced.id == 0);
        else 
        {
            REQUIRE(reveiced.id == 1);
            REQUIRE(reveiced.messageType == SyncBlink::Client::MESH_COUNTED);
            REQUIRE(reveiced.countedMessage.routeLedCount == _ledCount + 20);
            REQUIRE(reveiced.countedMessage.totalLedCount == _ledCount + (5*16) + 10 /* i increment */);
            REQUIRE(reveiced.countedMessage.routeNodeCount == 6);
            REQUIRE(reveiced.countedMessage.totalNodeCount == 16);
            REQUIRE(_waitFor == SyncBlink::Client::NONE);
        }
    }
}

TEST_CASE("Websocket Server collects mesh update successfully even with messages in between", "[Websocket]")
{
    _waitFor = SyncBlink::Client::MESH_UPDATED;
    _sendMessage = {1, SyncBlink::Server::MESH_UPDATE };

    handleReceivedMessage({2, SyncBlink::Client::EXTERNAL_ANALYZER }, 1);
    auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MESH_UPDATED }, 1);

    REQUIRE(reveiced.id == 1);
    REQUIRE(reveiced.messageType == SyncBlink::Client::MESH_UPDATED);
}