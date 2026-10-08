#if defined _VIREN_STORAGE_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_STORAGE_CORE_INCLUDED

#define VIREN_STORAGE_PATH 96
#define VIREN_STORAGE_LINE 160

stock VIREN_GetAccountPath(playerid, path[], size = sizeof(path))
{
    new name[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, name, sizeof(name));
    format(path, size, "scriptfiles/accounts/%s.ini", name);
    return 1;
}

stock bool:VIREN_AccountExists(playerid)
{
    new path[VIREN_STORAGE_PATH];
    VIREN_GetAccountPath(playerid, path);
    return fexist(path);
}

stock bool:VIREN_ReadAccountValue(playerid, const key[], value[], value_size)
{
    new path[VIREN_STORAGE_PATH];
    new line[VIREN_STORAGE_LINE];
    new file;
    VIREN_GetAccountPath(playerid, path);

    file = fopen(path, io_read);
    if (!file)
    {
        return false;
    }

    while (fread(file, line))
    {
        new key_len = strlen(key);
        if (strfind(line, key, true) == 0 && line[key_len] == '=')
        {
            strmid(value, line, key_len + 1, strlen(line), value_size);
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

stock VIREN_WriteAccount(playerid)
{
    new path[VIREN_STORAGE_PATH];
    new line[VIREN_STORAGE_LINE];
    new file;

    VIREN_GetAccountPath(playerid, path);
    file = fopen(path, io_write);
    if (!file)
    {
        printf("[VIREN] Failed to open account file for player %d.", playerid);
        return 0;
    }

    format(line, sizeof(line), "Version=1"); fwrite(file, line);
    format(line, sizeof(line), "PasswordHash=%s", VIREN_Player[playerid][VIREN_PasswordHash]); fwrite(file, line);
    format(line, sizeof(line), "Salt=%s", VIREN_Player[playerid][VIREN_Salt]); fwrite(file, line);
    format(line, sizeof(line), "CharacterName=%s", VIREN_Player[playerid][VIREN_CharacterName]); fwrite(file, line);
    format(line, sizeof(line), "Gender=%d", VIREN_Player[playerid][VIREN_Gender]); fwrite(file, line);
    format(line, sizeof(line), "Skin=%d", VIREN_Player[playerid][VIREN_Skin]); fwrite(file, line);
    format(line, sizeof(line), "Money=%d", VIREN_Player[playerid][VIREN_Money]); fwrite(file, line);
    format(line, sizeof(line), "Bank=%d", VIREN_Player[playerid][VIREN_Bank]); fwrite(file, line);
    format(line, sizeof(line), "Level=%d", VIREN_Player[playerid][VIREN_Level]); fwrite(file, line);
    format(line, sizeof(line), "XP=%d", VIREN_Player[playerid][VIREN_XP]); fwrite(file, line);

    fclose(file);
    return 1;
}

stock VIREN_LoadAccount(playerid)
{
    new value[96];

    VIREN_ReadAccountValue(playerid, "PasswordHash", VIREN_Player[playerid][VIREN_PasswordHash], 65);
    VIREN_ReadAccountValue(playerid, "Salt", VIREN_Player[playerid][VIREN_Salt], 65);

    if (VIREN_ReadAccountValue(playerid, "CharacterName", value, sizeof(value)))
    {
        format(VIREN_Player[playerid][VIREN_CharacterName], 32, "%s", value);
    }

    if (VIREN_ReadAccountValue(playerid, "Gender", value, sizeof(value)))
        VIREN_Player[playerid][VIREN_Gender] = strval(value);

    if (VIREN_ReadAccountValue(playerid, "Skin", value, sizeof(value)))
        VIREN_Player[playerid][VIREN_Skin] = strval(value);

    if (VIREN_ReadAccountValue(playerid, "Money", value, sizeof(value)))
        VIREN_Player[playerid][VIREN_Money] = strval(value);

    if (VIREN_ReadAccountValue(playerid, "Bank", value, sizeof(value)))
        VIREN_Player[playerid][VIREN_Bank] = strval(value);

    if (VIREN_ReadAccountValue(playerid, "Level", value, sizeof(value)))
        VIREN_Player[playerid][VIREN_Level] = strval(value);

    if (VIREN_ReadAccountValue(playerid, "XP", value, sizeof(value)))
        VIREN_Player[playerid][VIREN_XP] = strval(value);

    VIREN_Player[playerid][VIREN_HasCharacter] =
        (strlen(VIREN_Player[playerid][VIREN_CharacterName]) > 0);

    return 1;
}
