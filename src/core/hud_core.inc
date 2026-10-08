#if defined _VIREN_HUD_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_HUD_CORE_INCLUDED

new PlayerText:VIREN_Hud[MAX_PLAYERS];

stock VIREN_HudCreate(playerid)
{
    VIREN_Hud[playerid] = CreatePlayerTextDraw(playerid, 18.0, 420.0, "VIREN RP");
    PlayerTextDrawLetterSize(playerid, VIREN_Hud[playerid], 0.22, 1.0);
    PlayerTextDrawSetOutline(playerid, VIREN_Hud[playerid], 1);
    PlayerTextDrawBackgroundColor(playerid, VIREN_Hud[playerid], 0x00000066);
    PlayerTextDrawColor(playerid, VIREN_Hud[playerid], 0xFFFFFFFF);
    PlayerTextDrawUseBox(playerid, VIREN_Hud[playerid], true);
    PlayerTextDrawBoxColor(playerid, VIREN_Hud[playerid], 0x00000066);
    PlayerTextDrawTextSize(playerid, VIREN_Hud[playerid], 220.0, 0.0);
    PlayerTextDrawSetProportional(playerid, VIREN_Hud[playerid], true);
    return 1;
}

stock VIREN_HudUpdate(playerid)
{
    new text[160];

    format(
        text,
        sizeof(text),
        "VIREN RP | %s | LVL %d | XP %d | $%d | BANK $%d",
        VIREN_Player[playerid][VIREN_CharacterName],
        VIREN_Player[playerid][VIREN_Level],
        VIREN_Player[playerid][VIREN_XP],
        VIREN_Player[playerid][VIREN_Money],
        VIREN_Player[playerid][VIREN_Bank]
    );

    PlayerTextDrawSetString(playerid, VIREN_Hud[playerid], text);
    return 1;
}

stock VIREN_HudShow(playerid)
{
    PlayerTextDrawShow(playerid, VIREN_Hud[playerid]);
    return 1;
}

stock VIREN_HudDestroy(playerid)
{
    PlayerTextDrawDestroy(playerid, VIREN_Hud[playerid]);
    VIREN_Hud[playerid] = PlayerText:INVALID_TEXT_DRAW;
    return 1;
}
