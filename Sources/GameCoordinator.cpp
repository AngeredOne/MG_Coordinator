#include "GameCoordinator.h"
#include "ServerClient.h"
#include <boost/format.hpp>
#include <functional>

using namespace marxp;
#define BIND(X, Y) CoordinatorServer::Get().BindHandler(X, std::bind(&marxp::GameCoordinator::Y, this, std::placeholders::_1));
GameCoordinator::GameCoordinator()
{
    BIND(OP_CODES::GC_Games_Info, GetAllGamesInfo)
    BIND(OP_CODES::GC_Games_AllLobby, GetAllLobbiesInfoByGameId)
    BIND(OP_CODES::GC_Lobby_Create, CreateLobby)
    BIND(OP_CODES::GC_Lobby_Join, JoinLobby)
}
#undef BIND

void GameCoordinator::GetAllGamesInfo(client_ptr client)
{
    boost::format requestDB(R"_(SELECT * FROM "Games")_");
    auto responseDB = DataBaseWorker::Get().GetResponseByRequest(requestDB.str());
    struct GameData
    {
        uint16 gameId;
        char name[30];
        char desc[100];
    };

    auto ids = responseDB["id"];
    auto names = responseDB["name"];
    auto descs = responseDB["description"];

    GameData *allData = new GameData[names.size()];

    for (int i = 0; i < names.size(); ++i)
    {
        GameData data;
        std::string name = names[i].get();
        std::string desc = descs[i].get();
        strcpy(data.name, name.c_str());
        strcpy(data.desc, desc.c_str());
        data.gameId = atoi(ids[i].get());
        allData[i] = data;
    }

    SendDynamic(client->GetSocket(), allData, names.size());
    delete [] allData;
}

void GameCoordinator::GetAllLobbiesInfoByGameId(client_ptr client) {
    struct Lobby {
        uint32 lobbyid;
        uint32 playerscount;
    };
    
    auto gameId = ReadPacket<uint16>(client->GetSocket());
    auto lobbys = CoordinatorServer::Get().GetLobbiesInfoByGameId(*gameId.get());

    auto count = lobbys.size(); 

    Lobby * lobby = new Lobby[count];
    for (int i = 0; i < count; ++i) {
        lobby[i].lobbyid = lobbys[i].id;
        lobby[i].playerscount = lobbys[i].clientsHash.size();
    }
    SendDynamic(client->GetSocket(), lobby, count);
    delete [] lobby;

}

void GameCoordinator::GetLobbyInfoByLobbyId(client_ptr client) 
{

}

void GameCoordinator::CreateLobby(client_ptr client) 
{
    struct LobbyCreationInfo   {
        uint16 game;
    };
    auto info = ReadPacket<LobbyCreationInfo>(client->GetSocket());

    Lobby newLobby;
    newLobby.clientsHash.push_back(client->GetAuthToken());
    CoordinatorServer::Get().RegLobby(newLobby, info->game);

}

void GameCoordinator::JoinLobby(client_ptr client) 
{
    struct LobbyJoiningInfo   {
        uint32 game;
        uint32 lobbyid;
    };
    auto info = ReadPacket<LobbyJoiningInfo>(client->GetSocket());
    //GetLobbyByGameAndId
    //add
}