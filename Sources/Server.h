#include "MarxProtocol.h"
#include <thread>

namespace server
{

class CoordinatorServer
{
public:

    void Listen();

private:

    void HandleRequest(socket_ptr client);

    std::map<char[16], socket_ptr> usersTable;
};

} // namespace server