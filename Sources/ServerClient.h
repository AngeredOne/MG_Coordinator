#pragma once
#include <memory>
#include <iostream>
#include <queue>
#include "Server.h"

class LocalStream;
class ServerClient;
class StreamManager;

class ServerClient : public std::enable_shared_from_this<ServerClient>
{
public:
    uint64 authtoken;
    uint64 dbid;
    bool IsReconncted = false;

    ServerClient(uint64 token, uint64 DataBaseID);

    void InitClient(socket_ptr socket);
    const uint64 GetAuthToken();
    const socket_ptr GetSocket();
    const std::shared_ptr<StreamManager> GetStreamManager();

    //Lobbyfunctions
    bool JoinLobby(marxp::lobby_ptr lobby);
    void ExitLobby();

    template <class T>
    bool SendDynamic(T *structToSend, uint16 opcode, uint16 count)
    {

        boost::system::error_code ec;

        uint32 size = count * sizeof(T);
        char *toSendChar = new char[size + 8];

        std::memcpy(&toSendChar[0], &opcode, 2);
        std::memcpy(&toSendChar[4], &size, 4);
        std::memcpy(&toSendChar[8], structToSend, size);

       std::size_t length = socket->write_some(buffer(toSendChar, size + 8), ec);
        delete[] toSendChar;

        if (size > length)
        {
            throw marxp::ReadWriteBytesCountException();
        }
        return true;
    }

private:
    //Time for reconnect in seconds
    float timeForReconnect = 10;
    socket_ptr socket;
    std::shared_ptr<std::thread> clientListenerThread = nullptr;
    std::shared_ptr<StreamManager> streamManager = nullptr;
    std::weak_ptr<marxp::Lobby> inLobby;

    void WaitMessages();

    bool WaitToFullDisconnect();
};