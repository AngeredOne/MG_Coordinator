#include "GameCoordinator.h"
#include "ServerClient.h"
#include <boost/format.hpp>
#include <functional>
#include "Streams.h"

using namespace marxp;
#define BIND(X, Y) CoordinatorServer::Get().BindHandler(X, std::bind(&marxp::GameCoordinator::Y, this, std::placeholders::_1));
GameCoordinator::GameCoordinator()
{
    BIND(OP_CODES::GC_Games_Info, GetAllGamesInfo)
    BIND(OP_CODES::GC_Games_AllLobby, GetAllLobbiesInfoByGameId)
    BIND(OP_CODES::GC_Lobby_Create, CreateLobby)
    BIND(OP_CODES::GC_Lobby_Join, JoinLobby)
    BIND(OP_CODES::GC_Players_Info, GetInfoAboutPlayers)
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

    client->SendDynamic(allData, OP_CODES::GC_Games_Info, names.size());
    delete[] allData;
}

void GameCoordinator::GetAllLobbiesInfoByGameId(client_ptr client)
{
    struct Lobby
    {
        uint32 lobbyid;
        uint32 playerscount;
    };

    auto gameId = CoordinatorServer::Get().ReadPacket<uint16>(client->GetSocket());
    auto lobbys = CoordinatorServer::Get().GetLobbiesInfoByGameId(*gameId.get());

    auto count = lobbys.size();

    Lobby *lobby = new Lobby[count];
    for (int i = 0; i < count; ++i)
    {
        lobby[i].lobbyid = lobbys[i]->id;
        lobby[i].playerscount = lobbys[i]->clients.size();
    }
    client->SendDynamic(lobby,OP_CODES::GC_Games_AllLobby, count);
    delete[] lobby;
}

void GameCoordinator::GetLobbyInfoByLobbyId(client_ptr client)
{
}

void GameCoordinator::CreateLobby(client_ptr client)
{
    struct LobbyCreationInfo
    {
        uint16 game;
    };
    auto info = CoordinatorServer::Get().ReadPacket<LobbyCreationInfo>(client->GetSocket());

    lobby_ptr newLobby = std::make_shared<Lobby>();
    CoordinatorServer::Get().RegLobby(newLobby, info->game);
    
    client->JoinLobby(newLobby);
}

void GameCoordinator::JoinLobby(client_ptr client)
{
    struct LobbyJoiningInfo
    {
        uint16 game;
        uint32 lobbyid;
    };
    auto info = CoordinatorServer::Get().ReadPacket<LobbyJoiningInfo>(client->GetSocket());
    auto lobby = CoordinatorServer::Get().GetLobbyById(info->game, info->lobbyid);
    if (lobby)
    {
        client->JoinLobby(lobby);
    }
}

void GameCoordinator::GetInfoAboutPlayers(client_ptr client)
{
    auto stream = client->GetStreamManager()->GetStream(OP_CODES::GC_Players_Info);
    auto ids = stream->GetAllStructs<uint64>();
    std::stringstream stream_id;

    auto count = ids.size();
    if (count > 0)
    { 
        stream_id << std::to_string(*ids[0].get());
        for (int i = 1; i < count; ++i)
        {
            stream_id << "," << std::to_string(*ids[i].get());
        }
        boost::format requestDB(R"_(SELECT "Users"."Name" FROM "Users" WHERE "Users"."UserId" = ANY(ARRAY[%1%]) )_");
        requestDB % stream_id.str();
        auto responseDB = DataBaseWorker::Get().GetResponseByRequest(requestDB.str());
        if (responseDB.size() > 0)
        {
            struct UserData
            {
                char name[32];
            };

            UserData *users = new UserData[count]; 
            auto names = responseDB["Name"];
            for (int i = 0; i < names.size(); i++)
            {
                users[i] = UserData();
                auto s = std::string(names[i].get());
                strcpy(users[i].name, s.c_str() );
            }   
            client->SendDynamic(users, OP_CODES::GC_Players_Info, names.size());
        }
    }
}