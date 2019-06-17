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

    marxp::MarxInitRequest *initRequest = new marxp::MarxInitRequest;

    initRequest->command = 0x0001;
    marxp::SendPacket(uSocket, initRequest);

    std::string LogIn;
    std::string Pass;

    std::cout << "Enter Login:\n";
    getline(std::cin, LogIn, '\n');

    std::cout << "Enter Password:\n";
    getline(std::cin, Pass, '\n');



    initRequest->command = 0x0003;
    marxp::MyData *authData = new marxp::MyData;

    strcpy(authData->Test, LogIn.c_str());
    strcpy(authData->out, Pass.c_str());

    marxp::SendPacket(uSocket, initRequest);
    marxp::SendPacket(uSocket, authData);

    return 0;
}