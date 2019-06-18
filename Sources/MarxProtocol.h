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


template <class T>
T *ReadPacket(socket_ptr socket)
{
    T *packet = new T();
    size_t len = 0;
    len = socket->read_some(asio::buffer(packet, sizeof(T)));
    return packet;
}

template <class T>
bool SendPacket(socket_ptr socket, T *packetToSend)
{
    socket->write_some(asio::buffer(packetToSend, sizeof(T)));
}

} // namespace marxp