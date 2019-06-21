#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef io_service n_io_service; //net input\output service
typedef ip::tcp tcp;
typedef u_int16_t uint16;
typedef u_int32_t uint32;
typedef u_int64_t uint64;

template <class T>
std::shared_ptr<T> ReadPacket(socket_ptr socket)
{
    boost::system::error_code ec;

    auto sock = socket.get();
    std::shared_ptr<T> packet_ptr = std::make_shared<T>();

    std::size_t length = socket->read_some(buffer(packet_ptr.get(), sizeof(*packet_ptr.get())), ec);
    if (length < sizeof(T))
    {
        throw std::invalid_argument("The number of bytes received is less than expected.");
    }
    return packet_ptr;
}

template <class T>
bool SendPacket(socket_ptr socket, T *packetToSend)
{
    boost::system::error_code ec;

    auto sock = socket.get();

    std::size_t length = socket->write_some(buffer(packetToSend, sizeof(*packetToSend)), ec);
}

template <class T>
std::vector<std::shared_ptr<T>> ReadDynamic(socket_ptr socket)
{
    boost::system::error_code ec;

    auto count = ReadPacket<uint16>(socket);
    auto size = *count.get() * sizeof(T);

    char *collection_raw = new char[size];
    auto sock = socket.get();

    std::size_t length = socket->read_some(buffer(collection_raw, size), ec);

    if (length < size)
    {
        throw std::invalid_argument("The number of bytes received is less than expected.");
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
bool SendDynamic(socket_ptr socket, T *structToSend, uint16 count)
{
    boost::system::error_code ec;

    SendPacket(socket, &count);
    auto size = count * sizeof(T);

    char *toSendChar = new char[size];
    std::memcpy(toSendChar, structToSend, size);

    auto sock = socket.get();

    std::size_t length = socket->write_some(buffer(toSendChar, size), ec);
    delete[] toSendChar;
}

int main()
{
    tcp::endpoint tcpendpoint = tcp::endpoint(ip::address::from_string("127.0.0.1"), 25565);

    n_io_service service;
    socket_ptr uSocket = std::make_shared<tcp::socket>(tcp::socket(service));

    try
    {
        uSocket->connect(tcpendpoint);
        std::cout << "Client started>\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Cannot start client:\n"
                  << e.what() << '\n';
    }

    struct AuthData
    {
        char name[16];
        char pass[32];
    };

    struct Command
    {
        uint16 commandCode;
    };

    Command *initRequest = new Command;

    std::string LogIn;
    std::string Pass;

    std::cout << "Sending test data\n";

    AuthData *authData = new AuthData;

    strcpy(authData->name, "Nex");
    strcpy(authData->pass, "Password");

    SendPacket(uSocket, new uint16(0x00001));
    SendPacket(uSocket, authData);

    struct GameData
    {
        uint16 gameId;
        char name[30];
        char desc[100];
    };

    // Send "GetGames(GetAllGamesInfo)" request

    //Show info about playres
    SendPacket(uSocket, new uint16(11));
    uint64 *players = new uint64[1];
    players[0] = 1;
    //players[1] = 2;
    SendDynamic(uSocket, players, 2);
    struct UserData
    {
        char name[32];
    };
    auto userData = ReadDynamic<UserData>(uSocket);
    for (auto user : userData)
    {
        std::cout << std::string(user.get()->name, 32) << std::endl;
    }
    //CreateLobby with gameid 0
    SendPacket(uSocket, new uint16(0x00007));
    SendPacket(uSocket, new uint16(0));

    //CreateLobby with gameid 0
    SendPacket(uSocket, new uint16(0x00007));
    SendPacket(uSocket, new uint16(0));

    //CreateLobby with gameid 0
    SendPacket(uSocket, new uint16(0x00007));
    SendPacket(uSocket, new uint16(0));

    //CreateLobby with gameid 0
    SendPacket(uSocket, new uint16(0x00007));
    SendPacket(uSocket, new uint16(0));

    SendPacket(uSocket, new uint16(5));
    SendPacket(uSocket, new uint16(0));
    struct Lobby
    {
        uint32 lobbyid;
        uint32 playerscount;
    };
    auto lobbies = ReadDynamic<Lobby>(uSocket);

    for (auto lobby : lobbies)
    {
        std::cout << lobby->lobbyid << " " << lobby->playerscount << std::endl;
        ;
    }

    return 0;
}