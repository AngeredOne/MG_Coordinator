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

    try
    {

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

        //Show info about playres
        marxp::SendPacket(uSocket, new uint16(11));

        uint64 *players = new uint64[1];
        players[0] = 1;
        //players[1] = 2;

        marxp::SendDynamic(uSocket, players, 2);
        struct UserData
        {
            char name[32];
        };
        auto userData = marxp::ReadDynamic<UserData>(uSocket);

        for (auto user : userData)
        {
            std::cout << std::string(user.get()->name, 32) << std::endl;
        }

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
    }
    catch (...)
    {
        std::cout << "Catched";
    }

    return 0;
}