#include "Server.h"

namespace marxp
{
class GameCoordinator
{
public:
    GameCoordinator();
    
    //--GAMES INFO--
    //name, desc
    void GetAllGamesInfo(client_ptr);

    //PLAYERS
    void GetInfoAboutPlayers(client_ptr);

    //--LOBBYIS
    void CreateLobby(client_ptr);
    void JoinLobby(client_ptr);
    void GetAllLobbiesInfoByGameId(client_ptr);
    void GetLobbyInfoByLobbyId(client_ptr);
    
};
} // namespace marxp