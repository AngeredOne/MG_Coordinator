#pragma once
#include "Server.h"
#include <memory>

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

private:
    void WaitMessages();
    bool WaitToFullDisconnect();
    socket_ptr socket;
    std::shared_ptr<std::thread> clientListenerThread = nullptr;

    //Time for reconnect in seconds
    float timeForReconnect = 10;
};