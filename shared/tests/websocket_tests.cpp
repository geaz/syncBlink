#include <catch2/catch.hpp>
#include <iostream>
#include <map>

#include "network/websocket/messages/client_messages.hpp"
#include "network/websocket/messages/server_messages.hpp"

struct WaitInfo
{
    uint64_t sendId;
    uint16_t receivedAnswers;
    SyncBlink::Client::Message savedAnswer;
};

uint32_t _ledCount = 9;
std::map<uint64_t, WaitInfo> _waitInfos;

// Copy of the websocket server method.
// I know - not ideal, but this way is is easy testable on a PC
SyncBlink::Client::Message handleReceivedMessage(SyncBlink::Client::Message receivedMessage, uint32_t connectedClients)
{
    bool forwardMessage = false;
    auto iter = _waitInfos.find(receivedMessage.id);
    if(iter != _waitInfos.end())
    {
        auto& waitInfo = iter->second;
        waitInfo.receivedAnswers++;
        forwardMessage = waitInfo.receivedAnswers == connectedClients;

        switch (receivedMessage.messageType)
        {
        case SyncBlink::Client::MESH_COUNTED:
            if (waitInfo.savedAnswer.id == 0)
                waitInfo.savedAnswer = receivedMessage;
            else
            {
                waitInfo.savedAnswer.countedMessage.totalLedCount += receivedMessage.countedMessage.totalLedCount;
                waitInfo.savedAnswer.countedMessage.totalNodeCount += receivedMessage.countedMessage.totalNodeCount;
                if (waitInfo.savedAnswer.countedMessage.routeLedCount < receivedMessage.countedMessage.routeLedCount)
                    waitInfo.savedAnswer.countedMessage.routeLedCount = receivedMessage.countedMessage.routeLedCount;
                if (waitInfo.savedAnswer.countedMessage.routeNodeCount < receivedMessage.countedMessage.routeNodeCount)
                    waitInfo.savedAnswer.countedMessage.routeNodeCount = receivedMessage.countedMessage.routeNodeCount;
            }

            if(forwardMessage) // Last answer received
            {
                waitInfo.savedAnswer.countedMessage.totalNodeCount++;
                waitInfo.savedAnswer.countedMessage.totalLedCount += _ledCount;
                waitInfo.savedAnswer.countedMessage.routeNodeCount++;
                waitInfo.savedAnswer.countedMessage.routeLedCount += _ledCount;
            }
            break;
        default:
            waitInfo.savedAnswer = receivedMessage;
            break;
        }

        if (forwardMessage)
        {
            receivedMessage = waitInfo.savedAnswer;
            _waitInfos.erase(waitInfo.sendId);
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
    _waitInfos[1] = { 1, 0 };
    auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MOD_DISTRIBUTED }, 1);

    REQUIRE(reveiced.id == 1);
    REQUIRE(reveiced.messageType == SyncBlink::Client::MOD_DISTRIBUTED);
}

TEST_CASE("Websocket Server collects mod distribution for multiple clients successfully", "[Websocket]")
{
    _waitInfos[1] = { 1, 0 };
    for(uint32_t i = 0; i < 5; i++)
    {
        auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MOD_DISTRIBUTED }, 5);
        if(i != 4) REQUIRE(reveiced.id == 0);
        else 
        {
            REQUIRE(reveiced.id == 1);
            REQUIRE(reveiced.messageType == SyncBlink::Client::MOD_DISTRIBUTED);
            REQUIRE(_waitInfos.size() == 0);
        }
    }
}

TEST_CASE("Websocket Server collects mesh update for multiple clients successfully", "[Websocket]")
{
    _waitInfos[1] = { 1, 0 };
    for(uint32_t i = 0; i < 5; i++)
    {
        auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MESH_UPDATED }, 5);
        if(i != 4) REQUIRE(reveiced.id == 0);
        else 
        {
            REQUIRE(reveiced.id == 1);
            REQUIRE(reveiced.messageType == SyncBlink::Client::MESH_UPDATED);
            REQUIRE(_waitInfos.size() == 0);
        }
    }
}

TEST_CASE("Websocket Server should not forward not known collection answer", "[Websocket]")
{
    _waitInfos[1] = { 1, 0 };
    auto reveiced = handleReceivedMessage({2, SyncBlink::Client::MESH_UPDATED }, 5);
    REQUIRE(reveiced.id == 0);
}

TEST_CASE("Websocket Server collects mesh count successfully", "[Websocket]")
{
    _waitInfos[1] = { 1, 0 };
    SyncBlink::Client::Message receivedClientMessage = {1, SyncBlink::Client::MESH_COUNTED };
    receivedClientMessage.countedMessage = { 16, 1, 16, 1};

    auto reveiced = handleReceivedMessage(receivedClientMessage, 1);
    
    REQUIRE(reveiced.id == 1);
    REQUIRE(reveiced.messageType == SyncBlink::Client::MESH_COUNTED);
    REQUIRE(reveiced.countedMessage.routeLedCount == _ledCount + 16);
    REQUIRE(reveiced.countedMessage.totalLedCount == _ledCount + 16);
    REQUIRE(reveiced.countedMessage.routeNodeCount == 2);
    REQUIRE(reveiced.countedMessage.totalNodeCount == 2);
    REQUIRE(_waitInfos.size() == 0);
}

TEST_CASE("Websocket Server collects mesh count for multiple clients successfully", "[Websocket]")
{
    _waitInfos[1] = { 1, 0 };
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
            REQUIRE(_waitInfos.size() == 0);
        }
    }
}

TEST_CASE("Websocket Server collects mesh update successfully even with messages in between", "[Websocket]")
{
    _waitInfos[1] = { 1, 0 };
    handleReceivedMessage({2, SyncBlink::Client::EXTERNAL_ANALYZER }, 1);
    auto reveiced = handleReceivedMessage({1, SyncBlink::Client::MESH_UPDATED }, 1);

    REQUIRE(reveiced.id == 1);
    REQUIRE(reveiced.messageType == SyncBlink::Client::MESH_UPDATED);
}