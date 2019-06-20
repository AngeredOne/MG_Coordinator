#include "ServerClient.h"
#include <chrono>

using namespace marxp;

ServerClient::ServerClient(uint64 token, uint64 DataBaseID) : authtoken(token), dbid(DataBaseID)
{
    // Some specs here
}

void ServerClient::InitClient(socket_ptr sock)
{
    this->socket = sock;
    if (!clientListenerThread)
    {
        clientListenerThread = std::make_shared<std::thread>(&ServerClient::WaitMessages, this);
    }
}

void ServerClient::WaitMessages()
{
    while (true)
    {
        try
        {
            struct InitRequest
            {
                uint16 command;
            };

            auto request = marxp::ReadPacket<InitRequest>(socket);
            CoordinatorServer::Get().CallHandler(static_cast<OP_CODES>(request->command), shared_from_this());
        }
        catch(const std::invalid_argument &e)
        {
            std::cout << "SoSi HuI";
        }
        catch (const std::exception &e)
        {
            if(WaitToFullDisconnect())
            break;
        }
    }
}

bool ServerClient::WaitToFullDisconnect()
{
    float time = 0.f;
    while (!IsReconncted)
    {
        if (time >= timeForReconnect)
        {
            CoordinatorServer::Get().DisconnectClient(shared_from_this());
            return true;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            time += 100.f / 1000.f;
        }
    }
    return false;
}

const socket_ptr ServerClient::GetSocket()
{
    return socket;
}

const uint64 ServerClient::GetAuthToken() 
{
    return authtoken;
}