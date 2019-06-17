#include "DataBaseWorker.h"

DataBaseWorker::DataBaseWorker()
{

}

DataBaseWorker& DataBaseWorker::Get()
{
    static DataBaseWorker dbworker;
    return dbworker;
}

void DataBaseWorker::StartDataBase()
{
    std::cout << "DataBaseWorker starting.." << std::endl;
    std::cout << "Initializing connections.." << std::endl;
    std::cout << "\n";

    for(int i = 0; i < CONN_SIZE; ++i)
    {
        PGconn* conn_raw = PQsetdbLogin("127.0.0.1", "5432", NULL, NULL, "ProjectOffice", "Pilad", "qwerty");     
        std::shared_ptr<PGconn> conn_ptr(conn_raw, &PQfinish);
        Connection conn {conn_ptr, true};
        connections.push_back(conn);

        if(auto status = PQstatus(conn_raw); status == ConnStatusType::CONNECTION_STARTED)
        {
            std::cout << "Establishing connection " << i << "..." << std::endl;
        }
        else if(status == ConnStatusType::CONNECTION_OK)
        {
            std::cout << "Connection " << i << " ready." << std::endl;
        }
        else if(status == ConnStatusType::CONNECTION_BAD)
        {
            std::cout << "Connection " << i << " error!\nError massage: " << PQerrorMessage(conn_raw) << std::endl;
        }
    }

    std::cout << "\nDataBase ready!\n" << std::endl;
}

Connection DataBaseWorker::GetFreeConnection()
{
    while (true) {
        for (auto connection : connections) {
            if (connection.isAvilable) {
                return connection;
            }
        }
    }
}

void DataBaseWorker::FreeConnection(Connection conn)
{
    conn.isAvilable = true;   
}