#pragma once

#include <thread>
#include <list>
#include <cstdio>
#include <mutex>
#include <vector>
#include <iostream>
#include <type_traits>
#include <exception>
#include <boost/asio.hpp>
#include "DataBaseWorker.h"

class ServerClient;
using namespace boost::asio;

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef io_service n_io_service; //net input\output service
typedef ip::tcp tcp;
typedef u_int16_t uint16;
typedef u_int32_t uint32;
typedef u_int64_t uint64;
typedef std::shared_ptr<ServerClient> client_ptr;
typedef std::function<void(client_ptr)> HandlerDelegate;

namespace marxp
{

enum OP_CODES : uint16
{
    MAIN_SetConn = 1,
    MAIN_ReConn = 2,
    MAIN_Reg,
    GC_Games_Info,
    GC_Games_AllLobby,
    GC_Lobby_Info,
    GC_Lobby_Create,
    GC_Lobby_Join,
    GC_Lobby_Destroy,
    GC_Lobby_StartGame,
    GC_Players_Info,
    OP_CODE_LAST
};

struct ReadWriteBytesCountException : public std::exception
{
    const char *what() const throw()
    {
        return "The number of bytes (received\send) is less than expected.";
    }
};

struct Lobby
{
    uint32 id;
    uint32 gameid;
    std::list<uint64> clients;
};
typedef std::shared_ptr<Lobby> lobby_ptr;
typedef std::pair<uint32, lobby_ptr> LobbyInfo;
typedef std::map<uint16, std::map<LobbyInfo::first_type, LobbyInfo::second_type>> LobbyMap;

class CoordinatorServer
{

public:
    void Listen();

    void BindHandler(OP_CODES code, HandlerDelegate handler);
    void CallHandler(OP_CODES code, std::shared_ptr<ServerClient> client);

    void DisconnectClient(std::shared_ptr<ServerClient> client);

    static CoordinatorServer &Get();

    void WriteMessage(std::string msg);
    std::vector<lobby_ptr> GetLobbiesInfoByGameId(uint16 gameid);
    void RegLobby(lobby_ptr lb, uint16 gameid);
    void CloseLobby(uint32 gameid, uint32 lobbyid);
    lobby_ptr GetLobbyById(uint16 gameid, uint32 lobbyid);

    template <class T>
    std::shared_ptr<T> ReadPacket(socket_ptr socket, bool isBlockable = false)
    {
        boost::system::error_code ec;

        auto sock = socket.get();
        std::shared_ptr<T> packet_ptr = std::make_shared<T>();

        if (isBlockable)
        {
            read(*sock, buffer(packet_ptr.get(), sizeof(T)), ec);
        }
        else
        {
            std::size_t length = socket->read_some(buffer(packet_ptr.get(), sizeof(T)), ec);
            if (length < sizeof(T))
            {
                throw ReadWriteBytesCountException();
            }
        }

        return packet_ptr;
    }

    template <class T>
    bool SendPacket(socket_ptr socket, T *packetToSend)
    {
        boost::system::error_code ec;

        auto sock = socket.get();

        std::size_t length = socket->write_some(buffer(packetToSend, sizeof(T)), ec);
        if (sizeof(T) > length)
        {
            throw ReadWriteBytesCountException();
        }
    }

private:
    CoordinatorServer();

    CoordinatorServer(const CoordinatorServer &) = delete;
    CoordinatorServer operator=(const CoordinatorServer &) = delete;

    std::mutex coutMutex;

    std::map<uint64, std::shared_ptr<ServerClient>> clients;
    std::map<OP_CODES, HandlerDelegate> handlersTable;

    LobbyMap gamesLobbies;
    tcp::acceptor *acceptor;
    uint16 port = 25565;

    void HandleRequest(socket_ptr client);
    uint64 GetAuthToken();
};

} // namespace marxp