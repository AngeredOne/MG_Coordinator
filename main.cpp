#include <iostream>
#include "Server.h"

int main(int, char**) {

   server::CoordinatorServer coordinator = server::CoordinatorServer();
   coordinator.Listen();

}
