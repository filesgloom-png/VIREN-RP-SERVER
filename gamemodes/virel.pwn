#include <open.mp>

#include "../src/core/server_core.inc"
#include "../src/core/player_core.inc"

main()
{
    print("========================================");
    print(" VIREN RP - server core boot");
    print(" Build: 0.1.0");
    print("========================================");
}

public OnGameModeInit()
{
    return VIREN_ServerInit();
}

public OnGameModeExit()
{
    return VIREN_ServerShutdown();
}

public OnPlayerConnect(playerid)
{
    return VIREN_PlayerConnect(playerid);
}

public OnPlayerDisconnect(playerid, reason)
{
    return VIREN_PlayerDisconnect(playerid, reason);
}
