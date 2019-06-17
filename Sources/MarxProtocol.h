#pragma once
#include <vector>
#include <asio.hpp>
#include <iostream>
#include <type_traits>

typedef std::shared_ptr<asio::ip::tcp::socket> socket_ptr;
typedef asio::io_service n_io_service; //net input\output service
typedef asio::ip::tcp tcp;
typedef u_int16_t uint16;
typedef u_int32_t uint32;
typedef u_int64_t uint64;

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
    len = socket->read_some(asio::buffer(packet, sizeof(T)));
    return packet;
}

template <class T, class = typename std::enable_if<std::is_base_of<MarxPacket, T>::value>>
bool SendPacket(socket_ptr socket, T *packetToSend)
{
    socket->write_some(asio::buffer(packetToSend, sizeof(T)));
}

// std::string GetString(char arr[])
// {
//     std::string value = std::string(std::string(arr, sizeof(arr)).data());
//     return value;
// }

struct MyData : public marxp::MarxPacket
{
    char Test[16];
    char out[4];
};

} // namespace marxp