#pragma once

#include "MarxProtocol.h"
#include <thread>
#include <list>
#include "Lobby.h"
#include "DataBaseWorker.h"

class ServerClient;

namespace marxp
{

typedef std::function<void(std::shared_ptr<ServerClient>)> HandlerDelegate;

enum OP_CODES : uint32
{
    SetConn = 0x0001,
    ReConn = 0x0002,
    Auth = 0x0003,
    Reg = 0x00004,
    GetFriends,
    GetSessions,
    GetCurrentSession
};

class CoordinatorServer
{

public: 
    void Listen();

    void BindHandler(OP_CODES code, HandlerDelegate handler);
    void CallHandler(OP_CODES code, std::shared_ptr<ServerClient> client);

    void DisconnectClient(std::shared_ptr<ServerClient> client);

    static CoordinatorServer& Get();

private:

    CoordinatorServer();

    CoordinatorServer(const CoordinatorServer&) = delete;
    CoordinatorServer operator=(const CoordinatorServer&) = delete;

    std::map<uint64, std::shared_ptr<ServerClient>> clients;
    std::map<OP_CODES, HandlerDelegate> handlersTable;
    std::list<Lobby> lobbies;
    tcp::acceptor *acceptor;
    uint16 port = 25565;

    void HandleRequest(socket_ptr client);
    uint64 GetAuthToken();

    void WriteMessage(std::string msg);
};

} // namespace server