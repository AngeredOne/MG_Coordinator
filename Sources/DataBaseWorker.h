#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <postgresql/libpq-fe.h>

struct Connection
{
    std::shared_ptr<PGconn> pgconn;
    bool isAvilable = true;
};

class DataBaseWorker
{
    public:

    static DataBaseWorker& Get();
    void StartDataBase();

    Connection GetFreeConnection();
    void FreeConnection(Connection conn);

    private:

    DataBaseWorker();
    DataBaseWorker(const DataBaseWorker&) = delete;
    DataBaseWorker operator=(const DataBaseWorker&) = delete;

    std::list<Connection> connections;
    u_int32_t CONN_SIZE = 5;

};