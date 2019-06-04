#include "Server.h"

using namespace server;

void CoordinatorServer::Listen()
{
    n_io_service service; //boost io-service

    //tcp listener config
    tcp::endpoint listenEP(asio::ip::tcp::v4(), 7777);
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
    auto receivedData = protocol::ReadData(client);
}