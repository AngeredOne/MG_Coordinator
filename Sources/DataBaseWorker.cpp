#include "DataBaseWorker.h"
#include <iomanip>
#include <algorithm>

DataBaseWorker::DataBaseWorker()
{
}

DataBaseWorker &DataBaseWorker::Get()
{
    static DataBaseWorker dbworker;
    return dbworker;
}

void DataBaseWorker::StartDataBase()
{
    std::cout << "DataBaseWorker starting.." << std::endl;
    std::cout << "Initializing connections.." << std::endl;

    for (int i = 0; i < CONN_SIZE; ++i)
    {
        PGconn *conn_raw = PQsetdbLogin("5.167.17.110", "5432", NULL, NULL, "ProjectOffice", "CoordinatorServer", "SuperSecretPassword1337&");
        std::shared_ptr<PGconn> conn_ptr(conn_raw, &PQfinish);
        Connection conn{i, conn_ptr, true};
        connections.push_back(conn);

        if (auto status = PQstatus(conn_raw); status == ConnStatusType::CONNECTION_STARTED)
        {
            std::cout << "Establishing connection " << i << "..." << std::endl;
        }
        else if (status == ConnStatusType::CONNECTION_BAD)
        {
            std::cout << "Connection " << i << " error!\nError massage: " << PQerrorMessage(conn_raw) << std::endl;
        }
    }

    std::cout << connections.size() << " connections is ready to go!" << std::endl;;

    std::cout << "DataBaseWorker is done \n\n";
}

Connection DataBaseWorker::GetFreeConnection()
{
    while (true)
    {
        for (auto connection : connections)
        {
            if (connection.isAvailable)
            {
                connection.isAvailable = false;
                return connection;
            }
        }
    }
}

void DataBaseWorker::ExecuteRequest(std::string sql)
{
    auto conn = GetFreeConnection();
    PQsendQuery(conn.pgconn.get(), sql.c_str());
    conn.isAvailable = true;
}

TableResult DataBaseWorker::GetResponseByRequest(std::string sql)
{
    TableResult tableResult;
    auto conn = GetFreeConnection();

    //PQsendQuery(conn.pgconn.get(), sql.c_str());
    auto result = PQexec(conn.pgconn.get(), sql.c_str());
    //PQgetResult(conn.pgconn.get());

    int collumns = PQnfields(result);
    int rows = PQntuples(result);

    for (int i = 0; i < collumns; ++i)
    {
        std::string cName = PQfname(result, i);
        tableResult.insert(TableColumn(cName, ColumnStorage()));
        for (int j = 0; j < rows; ++j)
        {
            char *resValue = PQgetvalue(result, j, i);
            char *masd = new char[strlen(resValue)];
            strcpy(masd, resValue);
            tableResult[cName].push_back(char_ptr(masd));
        }
    }

    PQflush(conn.pgconn.get());
    PQclear(result);
    conn.isAvailable = true;

    return tableResult;
}

void DataBaseWorker::PrintResult(TableResult resultTable)
{
    std::cout << std::left;
    for (auto pair : resultTable)
    {
        std::cout << std::setw(20) << pair.first;
    }
    std::cout << std::endl;
    for (int i = 0; i < resultTable.begin()->second.size(); i++)
    {
        for (auto pair : resultTable)
        {
            std::string s = pair.second[i].get();
                while(auto s2 = s.find(" "))
                {
                    if (s2 == std::string::npos) {
                        break;
                    }
                    s.replace(s2, 1, "");
                    
                }
            std::cout << std::setw(20) << s;
        }
        std::cout << std::endl;
    }
}