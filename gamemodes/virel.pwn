#include <open.mp>

#include "../src/core/player_data.inc"
#include "../src/core/storage_core.inc"
#include "../src/core/character_core.inc"
#include "../src/core/account_core.inc"
#include "../src/core/hud_core.inc"
#include "../src/core/server_core.inc"
#include "../src/core/player_core.inc"

main()
{
    print("========================================");
    print(" VIREN RP - server core boot");
    print(" Build: 0.3.0");
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

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    return VIREN_PlayerDialogResponse(playerid, dialogid, response, listitem, inputtext);
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    return VIREN_PlayerCommand(playerid, cmdtext);
}
