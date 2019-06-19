#pragma once
#include <list>
struct Lobby
{
    uint32 id;
    std::list<int> clientsHash;
};