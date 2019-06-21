#include "Server.h"
#include "ServerClient.h"
#include <boost/format.hpp>
using namespace marxp;

void CoordinatorServer::Listen()
{    //Start and initialize DB
    DataBaseWorker::Get().StartDataBase();

    std::cout << "Getting all games ids and initializing lobbies map.\n";

    boost::format requestDB(R"_(SELECT "Games"."id" FROM "Games")_");
    auto responseDB = DataBaseWorker::Get().GetResponseByRequest(requestDB.str());

    for (auto gameId : responseDB["id"])
    {
        gamesLobbies.insert(std::pair<uint16, std::map<uint32, lobby_ptr>>(atoi(gameId.get()), std::map<uint32, lobby_ptr>()));
    }

    std::cout << "Lobbies map initialized. GamesCount: " << responseDB["id"].size() << std::endl;


    n_io_service service; //boost io-service

    //tcp listener config
    tcp::endpoint listenEP(ip::tcp::v4(), port);
    acceptor = new tcp::acceptor(service, listenEP);

    std::cout << "Server begin listening at port " << port << std::endl;
    while (true)
    {
        socket_ptr sock(new tcp::socket(service));
        acceptor->accept(*sock);
        std::cout << "Marx client request connection set. IP: " << sock->local_endpoint().address().to_string() << ":" << sock->local_endpoint().port() << std::endl;
        std::thread *handler = new std::thread(&CoordinatorServer::HandleRequest, this, sock);
    }
}

void CoordinatorServer::HandleRequest(socket_ptr client)
{
    try
    {
        struct InitRequest
        {
            uint16 command;
        };

        auto initialData = ReadPacket<InitRequest>(client);

        if (static_cast<OP_CODES>(initialData->command) == OP_CODES::MAIN_SetConn)
        {
            struct AuthData
            {
                char name[16];
                char pass[32];
            };
            auto data = ReadPacket<AuthData>(client);

            std::string name = std::string(std::string(data->name, sizeof(data->name)).c_str());
            std::string pass = std::string(std::string(data->pass, sizeof(data->pass)).c_str());

            boost::format authRequest(R"_(SELECT "IsUserExists"('%1%', '%2%'))_");
            authRequest % name;
            authRequest % pass;

            // Auth passed
            if (atoi(DataBaseWorker::Get().GetResponseByRequest(authRequest.str()).begin()->second[0].get()))
            {
                auto authtoken = GetAuthToken();

                boost::format DBIDRequest(R"_(SELECT "GetUserIdByName"('%1%'))_");
                DBIDRequest % name;
                // Get User.Id from Users table.
                uint64 dbid = atoi(DataBaseWorker::Get().GetResponseByRequest(DBIDRequest.str()).begin()->second[0].get());

                auto serverClient = std::make_shared<ServerClient>(authtoken, dbid);
                clients.insert(std::pair<uint64, std::shared_ptr<ServerClient>>(authtoken, serverClient));
                serverClient->InitClient(client);
            }
            else
            {
                goto CLOSE;
            }
        }
        else if (initialData->command == OP_CODES::MAIN_ReConn)
        {

            struct TokenData
            {
                uint64 token;
            };
            auto data = ReadPacket<TokenData>(client);

            //Reconnection
            if (auto value = clients.find(data->token); value != clients.end())
            {
                value->second->InitClient(client);
                value->second->IsReconncted = true;
            }
            else
            {
                goto CLOSE;
            }
        }
        else
        {
        CLOSE:
            client->shutdown(client->shutdown_both);
            client->close();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        client->shutdown(client->shutdown_both);
        client->close();
    }
}

void CoordinatorServer::WriteMessage(std::string msg)
{
    coutMutex.lock();
    std::cout << msg;
    coutMutex.unlock();
}

void CoordinatorServer::BindHandler(OP_CODES code, HandlerDelegate handler)
{
    handlersTable.insert(std::pair<OP_CODES, HandlerDelegate>(code, handler));
}

uint64 CoordinatorServer::GetAuthToken()
{
    static int i;
    return 0x21342112f1f2 + ++i;
}

CoordinatorServer::CoordinatorServer()
{
}

CoordinatorServer &CoordinatorServer::Get()
{
    static CoordinatorServer instance;
    return instance;
}

void CoordinatorServer::CallHandler(OP_CODES code, std::shared_ptr<ServerClient> client)
{
    if (auto handler = handlersTable.find(code); handler != handlersTable.end())
    {
        (*handler).second(client);
    }
}

void CoordinatorServer::DisconnectClient(std::shared_ptr<ServerClient> client)
{
    clients.erase(clients.find(client->authtoken));
    std::cout << "Deleted client!11!!\n";
}

std::vector<lobby_ptr> CoordinatorServer::GetLobbiesInfoByGameId(uint16 gameID)
{
    std::vector<lobby_ptr> lobbies;
    if (auto lobbiesList = gamesLobbies.find(gameID); lobbiesList != gamesLobbies.end())
    {
        for (auto lobby : (*lobbiesList).second)
        {
            lobbies.push_back(lobby.second);
        }
    }
    return lobbies;
}

void CoordinatorServer::RegLobby(lobby_ptr lb, uint16 gameId)
{
    static uint32 lid;

    lb->id = lid;
    LobbyInfo lInfo(lid, lb);
    lid++;

    if (auto gamePair = gamesLobbies.find(gameId); gamePair != gamesLobbies.end())
    {
        gamePair->second.insert(lInfo);
    }
}

lobby_ptr CoordinatorServer::GetLobbyById(uint16 gameid, uint32 lobbyid)
{
    if(auto gameLobby = gamesLobbies.find(gameid); gameLobby != gamesLobbies.end())
    {
        if(auto lobby = gameLobby->second.find(lobbyid); lobby != gameLobby->second.end())
        {
            return lobby->second;
        }
    }
}
void CoordinatorServer::CloseLobby(uint32 gameid, uint32 lobbyid) {
    if(auto gameLobby = gamesLobbies.find(gameid); gameLobby != gamesLobbies.end())
    {
        if(auto lobby = gameLobby->second.find(lobbyid); lobby != gameLobby->second.end())
        {
            gameLobby->second.erase(lobbyid);
        }
    }
}