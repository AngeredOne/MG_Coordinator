#include <iostream>
#include "Server.h"

int main(int, char**) {

   marxp::CoordinatorServer::Get().Listen();

}
