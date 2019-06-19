#pragma once

#include "MarxProtocol.h"
#include <thread>
#include <list>
#include "DataBaseWorker.h"
#include <cstdio>
#include <mutex>

class ServerClient;

typedef std::shared_ptr<ServerClient> client_ptr;
typedef std::function<void(client_ptr)> HandlerDelegate;

namespace marxp
{

enum OP_CODES : uint16
{
    MAIN_SetConn = 0x00001,
    MAIN_ReConn = 0x00002,
    MAIN_Reg,
    GC_Games_Info,
    GC_Games_AllLobby,
    GC_Lobby_Info,
    GC_Lobby_Create,
    GC_Lobby_Join,
    GC_Lobby_Destroy,
    GC_Lobby_StartGame
};  

struct Lobby
{
    uint32 id;
    std::list<int> clientsHash;
};
typedef std::pair<uint32, Lobby> LobbyInfo;
typedef std::map<uint16, std::map<LobbyInfo::first_type, LobbyInfo::second_type>> LobbyMap; 

class CoordinatorServer
{

public:
    void Listen();

    void BindHandler(OP_CODES code, HandlerDelegate handler);
    void CallHandler(OP_CODES code, std::shared_ptr<ServerClient> client);

    void DisconnectClient(std::shared_ptr<ServerClient> client);

    static CoordinatorServer &Get();

    void WriteMessage(std::string msg);
    std::vector<Lobby> GetLobbiesInfoByGameId(uint16 gameid);
    void RegLobby(Lobby lb, uint16 gameid);

private:
    CoordinatorServer();

    CoordinatorServer(const CoordinatorServer &) = delete;
    CoordinatorServer operator=(const CoordinatorServer &) = delete;

    std::mutex coutMutex;

    std::map<uint64, std::shared_ptr<ServerClient>> clients;
    std::map<OP_CODES, HandlerDelegate> handlersTable;
    
    LobbyMap gamesLobbies;
    tcp::acceptor *acceptor;
    uint16 port = 25565;

    void HandleRequest(socket_ptr client);
    uint64 GetAuthToken();
};

} // namespace marxp