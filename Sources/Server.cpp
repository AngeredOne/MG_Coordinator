#include "Server.h"

using namespace server;

void CoordinatorServer::Listen()
{
    n_io_service service; //boost io-service

    //tcp listener config
    tcp::endpoint listenEP(asio::ip::tcp::v4(), 25565);
    tcp::acceptor listener(service, listenEP);

    std::cout << "<Server started>\n";

    while (true)
    {
        socket_ptr sock(new tcp::socket(service));
        listener.accept(*sock);
        std::cout << "Client connected>\n";
        std::thread* handler = new std::thread(&CoordinatorServer::HandleRequest, this, sock);
    }
}

void CoordinatorServer::HandleRequest(socket_ptr client)
{
    auto initialData = marxp::ReadPacket<marxp::MarxInitRequest>(client);

    if(commandsTable.find(static_cast<OP_CODES>(initialData->command)) != commandsTable.end())
    {
        commandsTable[static_cast<OP_CODES>(initialData->command)](client);
    }
}

void CoordinatorServer::WriteMessage(std::string msg)
{
    std::cout << msg;
}

void CoordinatorServer::BindHandler(OP_CODES code, HandlerDelegate handler)
{
    commandsTable.insert(std::pair<OP_CODES, HandlerDelegate>(code, handler));
}