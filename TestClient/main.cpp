#include "Client.h"
#include <iostream>
#include <string>
#include <cstring>

int main()
{
    tcp::endpoint tcpendpoint = tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 25565);

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

    initRequest->commandCode = 0x0001;
    marxp::SendPacket(uSocket, initRequest);

    std::string LogIn;
    std::string Pass;

    std::cout << "Enter Login:\n";
    getline(std::cin, LogIn, '\n');

    std::cout << "Enter Password:\n";
    getline(std::cin, Pass, '\n');

    initRequest->commandCode = 0x0003;
    AuthData *authData = new AuthData;

    strcpy(authData->name, LogIn.c_str());
    strcpy(authData->pass, Pass.c_str());

    marxp::SendPacket(uSocket, authData);

    while(true)
    {
        try
        {
             auto req = marxp::ReadPacket<Command>(uSocket);
        }
        catch(const std::exception& e)
        {
            break;
            std::cerr << e.what() << '\n';
        }
    }

    return 0;
}