#pragma once

#include "MarxProtocol.h"
#include <thread>

namespace server
{

typedef std::function<void (socket_ptr)> HandlerDelegate;

enum OP_CODES : u_int32_t
{
    Auth = 0x0001,
    Reg = 0x00002
};

class CoordinatorServer
{

public:

    void Listen();
    void BindHandler(OP_CODES code, HandlerDelegate handler);

private:

    void HandleRequest(socket_ptr client);

    std::map<char[16], socket_ptr> usersTable;
    std::map<OP_CODES, HandlerDelegate> commandsTable;

    void WriteMessage(std::string msg);
};

} // namespace server