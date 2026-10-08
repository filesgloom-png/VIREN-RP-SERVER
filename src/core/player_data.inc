#if defined _VIREN_PLAYER_DATA_INCLUDED
    #endinput
#endif
#define _VIREN_PLAYER_DATA_INCLUDED

enum E_VIREN_PLAYER_DATA
{
    bool:VIREN_LoggedIn,
    bool:VIREN_HasCharacter,
    VIREN_AccountName[MAX_PLAYER_NAME + 1],
    VIREN_PasswordHash[65],
    VIREN_Salt[65],
    VIREN_CharacterName[32],
    VIREN_Gender,
    VIREN_Skin,
    VIREN_Money,
    VIREN_Bank,
    VIREN_Level,
    VIREN_XP,
    VIREN_LoginAttempts
};

new VIREN_Player[MAX_PLAYERS][E_VIREN_PLAYER_DATA];
new PlayerText:VIREN_Hud[MAX_PLAYERS];

stock VIREN_ResetPlayerData(playerid)
{
    VIREN_Player[playerid][VIREN_LoggedIn] = false;
    VIREN_Player[playerid][VIREN_HasCharacter] = false;
    VIREN_Player[playerid][VIREN_AccountName][0] = EOS;
    VIREN_Player[playerid][VIREN_PasswordHash][0] = EOS;
    VIREN_Player[playerid][VIREN_Salt][0] = EOS;
    VIREN_Player[playerid][VIREN_CharacterName][0] = EOS;
    VIREN_Player[playerid][VIREN_Gender] = 0;
    VIREN_Player[playerid][VIREN_Skin] = 26;
    VIREN_Player[playerid][VIREN_Money] = 0;
    VIREN_Player[playerid][VIREN_Bank] = 0;
    VIREN_Player[playerid][VIREN_Level] = 1;
    VIREN_Player[playerid][VIREN_XP] = 0;
    VIREN_Player[playerid][VIREN_LoginAttempts] = 0;
    VIREN_Hud[playerid] = PlayerText:INVALID_TEXT_DRAW;
    return 1;
}
