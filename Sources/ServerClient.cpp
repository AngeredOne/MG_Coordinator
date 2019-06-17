#include "ServerClient.h"

using namespace marxp;

ServerClient::ServerClient(socket_ptr socket) : socket(socket)
{
    clientListenerThread = std::make_shared<std::thread>(&ServerClient::Listen, this);
}

void ServerClient::Init()
{
    clientListenerThread = std::make_shared<std::thread>(&ServerClient::Listen, this);
}

void ServerClient::Listen()
{
    while (true)
    {
        try
        {
            auto request = marxp::ReadPacket<marxp::MarxInitRequest>(socket);
            CoordinatorServer::Get().CallHandler(static_cast<OP_CODES>(request->command), shared_from_this());
        }
        catch (const std::exception &e)
        {
            CoordinatorServer::Get().DisconnectClient(shared_from_this());
            break;
        }
    }
}

void ServerClient::WaitToFullDisconnect()
{
    //Waiter
}