#pragma once
#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include <type_traits>
#include <exception>

using namespace boost::asio;

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef io_service n_io_service; //net input\output service
typedef ip::tcp tcp;
typedef u_int16_t uint16;
typedef u_int32_t uint32;
typedef u_int64_t uint64;

namespace marxp
{

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
bool SendPacket(socket_ptr socket, T *packetToSend)
{
    boost::system::error_code ec;

    auto sock = socket.get();

    std::size_t length = socket->write_some(buffer(packetToSend, sizeof(*packetToSend)), ec);
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

} // namespace marxp