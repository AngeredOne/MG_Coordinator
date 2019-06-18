#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <cstring>
#include <postgresql/libpq-fe.h>

struct Connection
{
    uint16_t id = 0;
    std::shared_ptr<PGconn> pgconn;
    bool isAvailable = true;
};
typedef std::shared_ptr<char> char_ptr;
typedef std::vector<char_ptr> ColumnStorage;
typedef std::pair<std::string, ColumnStorage> TableColumn;
typedef std::map<TableColumn::first_type, TableColumn::second_type> TableResult;

class DataBaseWorker
{
public:
    static DataBaseWorker &Get();
    void StartDataBase();

    TableResult GetResponseByRequest(std::string sqlRequest);
    void ExecuteRequest(std::string sqlRequest);

    void PrintResult(TableResult toPrint);

private:
    DataBaseWorker();
    DataBaseWorker(const DataBaseWorker &) = delete;
    DataBaseWorker operator=(const DataBaseWorker &) = delete;

    std::list<Connection> connections;
    u_int32_t CONN_SIZE = 5;

    Connection GetFreeConnection();
};