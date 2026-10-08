#if defined _VIREN_PLAYER_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_PLAYER_CORE_INCLUDED

stock VIREN_PlayerConnect(playerid)
{
    SendClientMessage(playerid, 0xFFFFFFFF, "Добро пожаловать на VIREN RP.");
    SendClientMessage(playerid, 0xFFFFFFFF, "Серверный фундамент загружен. RP-системы находятся в разработке.");

    printf("[VIREN] Player %d connected.", playerid);
    return 1;
}

stock VIREN_PlayerDisconnect(playerid, reason)
{
    printf("[VIREN] Player %d disconnected. Reason: %d.", playerid, reason);
    return 1;
}
