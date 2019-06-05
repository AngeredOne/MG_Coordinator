#pragma once
#include <vector>
#include <asio.hpp>
#include <iostream>
#include <type_traits>

typedef std::shared_ptr<asio::ip::tcp::socket> socket_ptr;
typedef asio::io_service n_io_service; //net input\output service
typedef asio::ip::tcp tcp;

namespace marxp
{

struct MarxPacket
{
};

// Size: 2b;
struct MarxInitPacket : public MarxPacket
{
    u_int16_t bytesCount = 0x00;
};

// Size: 20b;
struct MarxInitRequest : public MarxPacket
{
    u_int32_t command;
    char authToken[16];
};

struct MarxData : public MarxPacket
{
    char data[128];
};

template <class T, class = typename std::enable_if<std::is_base_of<MarxPacket, T>::value>>
T *ReadPacket(socket_ptr socket)
{
    T *packet = new T();
    size_t len = 0;
    try
    {
        len = socket->read_some(asio::buffer(packet, sizeof(T)));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error while reading data from netStream:\n"
                  << e.what() << '\n';
    }
    return packet;
}

template <class T, class = typename std::enable_if<std::is_base_of<MarxPacket, T>::value>>
bool SendPacket(socket_ptr socket, T *packetToSend)
{
    socket->write_some(asio::buffer(packetToSend, sizeof(T)));
}

struct MyData : public marxp::MarxPacket
{

    std::string Test = "";
    char out[4];
};

} // namespace marxp