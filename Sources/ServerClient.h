#pragma once
#include "Server.h"
#include <memory>

class ServerClient : public std::enable_shared_from_this<ServerClient>
{
    public:
    uint64 authtoken;
    bool IsNeedToDelete = false;
    ServerClient(socket_ptr socket);
    void Init();
    private:
    void Listen();
    void WaitToFullDisconnect();
    std::weak_ptr<Lobby> inLobby;    
    socket_ptr socket;
    std::shared_ptr<std::thread> clientListenerThread = nullptr;
};