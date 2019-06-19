#include <iostream>
#include "GameCoordinator.h"

int main(int, char**) {

   marxp::GameCoordinator *gCoord = new marxp::GameCoordinator();
   marxp::CoordinatorServer::Get().Listen();
}
