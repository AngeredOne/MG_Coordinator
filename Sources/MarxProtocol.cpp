#include "MarxProtocol.h"
using namespace protocol;

protocol::MarxPacket * protocol::ReadPacket(socket_ptr socket)
{
    protocol::MarxPacket *packet = new MarxPacket();

    size_t len = 0;
    try
    {
        len = socket->read_some(asio::buffer(packet, sizeof(MarxPacket)));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error while reading data from netStream:\n"
                  << e.what() << '\n';
    }

    return packet;
};

std::vector<char> protocol::ReadData(socket_ptr socket)
{
    std::vector<char> data;
    u_int16_t bytesRecieved = 0;
    protocol::MarxPacket* packet;

    do
    {
        packet = ReadPacket(socket); // Read packet

        u_int16_t neededToRead = packet->bytesCount - bytesRecieved;

        if (neededToRead > 128)
            neededToRead = 128;

        for (int i = 0; i < neededToRead; ++i)
            data.push_back(packet->data[i]);
        bytesRecieved += neededToRead;
    } while (packet->bytesCount > bytesRecieved);

        return data;
};