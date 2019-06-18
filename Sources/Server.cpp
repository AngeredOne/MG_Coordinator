#include "Server.h"
#include "ServerClient.h"
#include <boost/format.hpp>
using namespace marxp;

void CoordinatorServer::Listen()
{
    n_io_service service; //boost io-service

    //tcp listener config
    tcp::endpoint listenEP(asio::ip::tcp::v4(), port);
    acceptor = new tcp::acceptor(service, listenEP);

    std::cout << "Server started.\n\n";

    //Start and initialize DB
    DataBaseWorker::Get().StartDataBase();

    std::cout << "Server begin listening." << std::endl;
    while (true)
    {
        socket_ptr sock(new tcp::socket(service));
        acceptor->accept(*sock);
        std::cout << "Marx connect request from: " << sock << std::endl;
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

        auto initialData = marxp::ReadPacket<InitRequest>(client);

        if (static_cast<OP_CODES>(initialData->command) == OP_CODES::SetConn)
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
        else if (initialData->command == OP_CODES::ReConn)
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
    std::cout << msg;
}

void CoordinatorServer::BindHandler(OP_CODES code, HandlerDelegate handler)
{
    handlersTable.insert(std::pair<OP_CODES, HandlerDelegate>(code, handler));
}

uint64 CoordinatorServer::GetAuthToken()
{
    return 0x21342112f1f2;
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