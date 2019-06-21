#pragma once
#include <memory>
#include "Server.h"

class ServerClient : public std::enable_shared_from_this<ServerClient>
{
public:
    uint64 authtoken;
    uint64 dbid;
    bool IsReconncted = false;

    ServerClient(uint64 token, uint64 DataBaseID);

    void InitClient(socket_ptr socket);
    const uint64 GetAuthToken();
    const socket_ptr GetSocket();
    //Lobbyfunctions
    bool JoinLobby(marxp::lobby_ptr lobby);
    void ExitLobby();

    template <class T>
    std::vector<std::shared_ptr<T>> ReadDynamic()
    {
        boost::system::error_code ec;

        auto count = marxp::CoordinatorServer::Get().ReadPacket<uint16>(socket, true);
        auto size = *count.get() * sizeof(T);

        char *collection_raw = new char[size];

        std::size_t length = socket->read_some(buffer(collection_raw, size), ec);

        if (length < size)
        {
            throw marxp::ReadWriteBytesCountException();
        }

        T *collection = reinterpret_cast<T *>(collection_raw);

        std::vector<std::shared_ptr<T>> out;

        for (int i = 0; i < *count.get(); ++i)
        {
            out.push_back(std::make_shared<T>(collection[i]));
        }

        return out;
    }

    template <class T>
    bool SendDynamic(T *structToSend, uint16 count)
    {
        boost::system::error_code ec;

        marxp::CoordinatorServer::Get().SendPacket(socket, &count);
        auto size = count * sizeof(T);

        char *toSendChar = new char[size];
        std::memcpy(toSendChar, structToSend, size);

        std::size_t length = socket->write_some(buffer(toSendChar, size), ec);
        delete[] toSendChar;

        if (size > length)
        {
            throw marxp::ReadWriteBytesCountException();
        }
    }

private:
    //Time for reconnect in seconds
    float timeForReconnect = 10;
    socket_ptr socket;
    std::shared_ptr<std::thread> clientListenerThread = nullptr;
    std::weak_ptr<marxp::Lobby> inLobby;

    void WaitMessages();
    bool WaitToFullDisconnect();
};