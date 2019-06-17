#include "Server.h"
#include "ServerClient.h"


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
        std::thread* handler = new std::thread(&CoordinatorServer::HandleRequest, this, sock);
    }
}

void CoordinatorServer::HandleRequest(socket_ptr client)
{
    auto initialData = marxp::ReadPacket<marxp::MarxInitRequest>(client);

    if(static_cast<OP_CODES>(initialData->command) == OP_CODES::SetConn)
    {
        uint64 authtoken = GetAuthToken();
        clients.insert( std::pair<uint64, std::shared_ptr<ServerClient>>( authtoken, std::make_shared<ServerClient>(client) ) );
        //send to client authtoken
    }
    else if(initialData->command == OP_CODES::ReConn)
    {
        //Reconnection
        // if(auto value = clients.find(initialData->authToken); value != clients.end())
        // {
        //     value->second->IsNeedToDelete = true;
        //     value->second.reset();
        //     value->second = std::make_shared<ServerClient>(client);
        // }
    }
    else
    {
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

CoordinatorServer::CoordinatorServer() {

}

CoordinatorServer& CoordinatorServer::Get()
{
    static CoordinatorServer instance;
    return instance;
} 

void CoordinatorServer::CallHandler(OP_CODES code, std::shared_ptr<ServerClient> client)
{
    if(auto handler = handlersTable.find(code); handler != handlersTable.end())
    {
        (*handler).second(client);
    }
}

void CoordinatorServer::DisconnectClient(std::shared_ptr<ServerClient> client)
{
    clients.erase(clients.find(client->authtoken));
    std::cout << "Deleted client!11!!\n" ;
}