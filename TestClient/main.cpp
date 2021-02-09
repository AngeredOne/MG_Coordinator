#include <ios>
#include <iostream>
#include <streambuf>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <thread>
#include <boost/asio.hpp>

#include "Stream.h"

using namespace boost::asio;

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef io_service n_io_service; //net input\output service
typedef ip::tcp tcp;
typedef char byte;
typedef u_int16_t uint16;
typedef u_int32_t uint32;
typedef u_int64_t uint64;

template <class T>
std::shared_ptr<T> ReadPacket(socket_ptr socket)
{
    boost::system::error_code ec;

    auto sock = socket.get();
    std::shared_ptr<T> packet_ptr = std::make_shared<T>();

    std::size_t length = socket->read_some(buffer(packet_ptr.get(), sizeof(T)), ec);
    if (length < sizeof(T))
    {
        throw std::invalid_argument("The number of bytes received is less than expected.");
    }
    return packet_ptr;
}

template <class T>
void ReadStream(socket_ptr socket, std::iostream ios)
{
    auto packet = new T();
    std::size_t length = socket->read_some(buffer(packet, sizeof(T)));

    if (length < sizeof(T))
    {
        throw std::invalid_argument("The number of bytes received is less than expected.");
    }

    ios << packet;
}

template <class T>
bool SendPacket(socket_ptr socket, T *packetToSend)
{
    boost::system::error_code ec;

    auto sock = socket.get();

    std::size_t length = socket->write_some(buffer(packetToSend, sizeof(*packetToSend)), ec);
}

template <class T>
void ReadDynamicFromStream(socket_ptr socket, std::iostream *stream)
{
    auto count = ReadPacket<uint16>(socket);
    auto size = *count.get() * sizeof(T);

    char *collection_raw = new char[size];

    std::size_t length = socket->read_some(buffer(collection_raw, size));

    if (length < size)
    {
        throw std::invalid_argument("The number of bytes received is less than expected.");
    }

    stream->write(collection_raw, size);
}

template <class T>
bool SendDynamic(socket_ptr socket, T *structToSend, uint16 count)
{

    boost::system::error_code ec;

    auto size = count * sizeof(T);
    std::size_t length = socket->write_some(buffer(structToSend, size), ec);

    if (size > length)
    {
        throw std::invalid_argument("Nu tupiiiieeeee");
    }
    return true;
}

void GetNextPacket(socket_ptr socket, StreamManager *streamManager)
{
    struct SystemData
    {
        uint16 opcode;
        uint32 length;
    };

    while (true)
    {
        // Read system block of raw bytes: code and lenght
        char *data = new char[sizeof(SystemData)];
        auto readSize = read(*socket, buffer(data, sizeof(SystemData)));
        //vse ok
        // Read data which bounded with that system block
        auto sysData = reinterpret_cast<SystemData *>(data);
        data = new char[sysData->length];
        readSize = read(*socket, buffer(data, sysData->length));
        streamManager->ProceedDataOnPort(sysData->opcode, data, sysData->length);
    }
}

int main()
{

    struct AlligmentWith
    {
        uint16 var1;
        uint32 var2;
    };


    #pragma pack(push, 1)
    struct AlligmentWithOut
    {
        uint16 var1;
        uint32 var2;
    };
    #pragma pack(pop)

    std::cout << "Size of structure with alligment is: " << sizeof(AlligmentWith) << std::endl;
    std::cout << "Size of structure withofut alligment is: " << sizeof(AlligmentWithOut) << std::endl;
    return 0;

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
    std::cout << "Sending test data\n";
    AuthData *authData = new AuthData;
    strcpy(authData->name, "Nex");
    strcpy(authData->pass, "Password");

    SendPacket(uSocket, new uint16(0x00001));
    SendPacket(uSocket, authData);

    StreamManager *streamManager = new StreamManager;
    streamManager->Start();

    std::thread *wmsg = new std::thread(GetNextPacket, uSocket, streamManager);

    // Send "GetGames(GetAllGamesInfo)" request
    SendPacket(uSocket, new uint32(4));
    SendPacket(uSocket, new uint32(0));

    //PlayersInfo
    SendPacket(uSocket, new uint32(11));
    SendPacket(uSocket, new uint32(0));
    uint64 *desiredPlayers = new uint64[2];
    desiredPlayers[0] = 1;
    desiredPlayers[1] = 2;
    SendPacket(uSocket, new uint32(11));
    SendPacket(uSocket, new uint32(sizeof(uint64) * 2));
    SendDynamic(uSocket, desiredPlayers, 2);

    struct UserData
    {
        char name[32];
    };
    auto userData = streamManager->GetStream(11)->GetAllStructs<UserData>();
    for (auto data : userData)
    {
        std::cout << data->name << std::endl;
    }

    struct GameData
    {
        uint16 gameId;
        char name[30];
        char desc[100];
    };
    auto gameData = streamManager->GetStream(4)->GetAllStructs<GameData>();
    for (auto data : gameData)
    {
        std::cout << data->name << " . " << data->desc << std::endl;
    }
    return 0;
}