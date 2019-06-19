#include "Client.h"
#include <iostream>
#include <string>
#include <cstring>

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

    marxp::SendPacket(uSocket, new uint16(0x00001));
    marxp::SendPacket(uSocket, authData);

    struct GameData
    {
        uint16 gameId;
        char name[30];
        char desc[100];
    };

    // Send "GetGames(GetAllGamesInfo)" request
    marxp::SendPacket(uSocket, new uint16(0x00004));
    auto allGames = marxp::ReadDynamic<GameData>(uSocket);

    //CreateLobby with gameid 0
    marxp::SendPacket(uSocket, new uint16(0x00007));
    marxp::SendPacket(uSocket, new uint16(0));

    //CreateLobby with gameid 0
    marxp::SendPacket(uSocket, new uint16(0x00007));
    marxp::SendPacket(uSocket, new uint16(0));

    //CreateLobby with gameid 0
    marxp::SendPacket(uSocket, new uint16(0x00007));
    marxp::SendPacket(uSocket, new uint16(0));

    //Show all lobbies by gameid 0
    marxp::SendPacket(uSocket, new uint16(5));
    marxp::SendPacket(uSocket, new uint16(0));

    struct Lobby
    {
        uint32 lobbyid;
        uint32 playerscount;
    };

    auto lobbies = marxp::ReadDynamic<Lobby>(uSocket);
    std::cout << "Lobbies list:\n";
    for (auto lb : lobbies)
    {
        std::cout << "Lobby: " << lb->lobbyid << " with " << lb->playerscount << " players\n";
    }

    return 0;
}