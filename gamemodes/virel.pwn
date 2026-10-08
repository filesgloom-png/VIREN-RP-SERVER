#include <open.mp>

#include "../src/core/player_data.inc"
#include "../src/core/storage_core.inc"
#include "../src/core/character_core.inc"
#include "../src/core/account_core.inc"
#include "../src/core/hud_core.inc"
#include "../src/core/economy_core.inc"
#include "../src/core/rp_core.inc"
#include "../src/core/job_core.inc"
#include "../src/core/faction_core.inc"
#include "../src/core/lspd_core.inc"
#include "../src/core/admin_core.inc"
#include "../src/core/server_core.inc"
#include "../src/core/player_core.inc"

main()
{
    print("========================================");
    print(" VIREN RP - server core boot");
    print(" Build: 0.9.0");
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
    VIREN_LSPD_OnDisconnect(playerid);
    return VIREN_PlayerDisconnect(playerid, reason);
}

public OnPlayerSpawn(playerid)
{
    return VIREN_LSPD_OnSpawn(playerid);
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    return VIREN_PlayerDialogResponse(playerid, dialogid, response, listitem, inputtext);
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    return VIREN_PlayerCommand(playerid, cmdtext);
}
