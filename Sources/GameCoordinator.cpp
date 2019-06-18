#include "GameCoordinator.h"
#include "ServerClient.h"
#include <boost/format.hpp>
#include <functional>

using namespace marxp;


GameCoordinator::GameCoordinator() {
    CoordinatorServer::Get().BindHandler(OP_CODES::Reg, std::bind(&marxp::GameCoordinator::GetAllGamesInfo, this, std::placeholders::_1));
}

void GameCoordinator::GetAllGamesInfo(std::shared_ptr<ServerClient> client)
{
    boost::format requestDB(R"_(SELECT * FROM "Games")_");    
    auto responseDB = DataBaseWorker::Get().GetResponseByRequest(requestDB.str());

    struct GameData {
        char name [30];
        char desc [100];
    };
    

}