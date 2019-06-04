#include <vector>
#include <asio.hpp>
#include <iostream>

typedef std::shared_ptr<asio::ip::tcp::socket> socket_ptr;
typedef asio::io_service n_io_service; //net input\output service
typedef asio::ip::tcp tcp;

namespace protocol
{

struct MarxPacket
{
    u_int16_t bytesCount;
    // 2 bytes means 65536 max bytes and 4bytes OP-Code of command

    char data[128];
};

MarxPacket *ReadPacket(socket_ptr socket);
std::vector<char> ReadData(socket_ptr socket);

} // namespace Protocol