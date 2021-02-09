#include "ServerClient.h"
#include "Streams.h"
#include <chrono>

using namespace marxp;

ServerClient::ServerClient(uint64 token, uint64 DataBaseID) : authtoken(token), dbid(DataBaseID)
{
    // Some specs here
}

void ServerClient::InitClient(socket_ptr sock)
{
    this->socket = sock;
    if (!streamManager)
        streamManager = std::make_shared<StreamManager>();
    streamManager->Start(shared_from_this());
    if (!clientListenerThread)
    {
        clientListenerThread = std::make_shared<std::thread>(&ServerClient::WaitMessages, this);
    }
}

void ServerClient::WaitMessages()
{

    struct SystemData
    {
        uint16 opcode;
        uint32 length;
    };

    while (true)
    {
        try
        {
            // Read system block of raw bytes: code and lenght
            char *data = new char[8];
            auto readSize = read(*socket, buffer(data, 8));

            if (readSize < 8)
                throw ReadWriteBytesCountException();

            // Read data which bounded with that system block
            auto sysData = reinterpret_cast<SystemData *>(data);
            data = new char[sysData->length];
            readSize = read(*socket, buffer(data, sysData->length));
            if (readSize < sysData->length)
                throw ReadWriteBytesCountException();
            streamManager->ProceedDataOnPort(sysData->opcode, data, sysData->length);
        }
        catch (std::exception &e)
        {
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

const std::shared_ptr<StreamManager> ServerClient::GetStreamManager()
{
    return streamManager;
}

const uint64 ServerClient::GetAuthToken()
{
    return authtoken;
}

bool ServerClient::JoinLobby(lobby_ptr lobby)
{
    ExitLobby();
    lobby->clients.push_back(authtoken);
    inLobby = lobby;

    return true;
}

void ServerClient::ExitLobby()
{
    if (auto existsLobby = inLobby.lock())
    {
        existsLobby->clients.remove(authtoken);
        if (existsLobby->clients.size() == 0)
        {
            CoordinatorServer::Get().CloseLobby(existsLobby->gameid, existsLobby->id);
        }
        inLobby = std::weak_ptr<Lobby>();
    }
}