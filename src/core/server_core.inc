#if defined _VIREN_SERVER_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_SERVER_CORE_INCLUDED

#define VIREN_VERSION "0.4.0"
#define VIREN_NAME "VIREN RP"

stock VIREN_ServerInit()
{
    SetGameModeText(VIREN_NAME " " VIREN_VERSION);
    ShowPlayerMarkers(PLAYER_MARKERS_MODE_GLOBAL);
    ShowNameTags(true);
    EnableStuntBonusForAll(false);
    DisableInteriorEnterExits();

    print("[VIREN] Server core initialized.");
    print("[VIREN] v0.4 economy and RP systems loaded.");
    return 1;
}

stock VIREN_ServerShutdown()
{
    print("[VIREN] Server core shutdown.");
    return 1;
}
