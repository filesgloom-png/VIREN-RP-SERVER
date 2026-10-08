#if defined _VIREN_PLAYER_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_PLAYER_CORE_INCLUDED

stock VIREN_PlayerConnect(playerid)
{
    VIREN_ResetPlayerData(playerid);

    new name[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, name, sizeof(name));
    format(VIREN_Player[playerid][VIREN_AccountName], MAX_PLAYER_NAME + 1, "%s", name);

    TogglePlayerSpectating(playerid, true);

    SendClientMessage(playerid, 0xFFFFFFFF, "Добро пожаловать на VIREN RP.");

    if (VIREN_AccountExists(playerid))
    {
        VIREN_LoadAccount(playerid);
        SendClientMessage(playerid, 0xFFFF66FF, "Аккаунт найден. Требуется авторизация.");
        VIREN_ShowLogin(playerid);
    }
    else
    {
        SendClientMessage(playerid, 0x66FF66FF, "Аккаунт не найден. Пройдите регистрацию.");
        VIREN_ShowRegister(playerid);
    }

    printf("[VIREN] Player %d connected as %s.", playerid, name);
    return 1;
}

stock VIREN_PlayerDisconnect(playerid, reason)
{
    if (VIREN_Player[playerid][VIREN_LoggedIn])
    {
        VIREN_WriteAccount(playerid);
    }

    VIREN_HudDestroy(playerid);
    printf("[VIREN] Player %d disconnected. Reason: %d.", playerid, reason);
    VIREN_ResetPlayerData(playerid);
    return 1;
}

stock VIREN_PlayerDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    if (dialogid == VIREN_DIALOG_REGISTER_PASSWORD)
    {
        if (!response)
        {
            Kick(playerid);
            return 1;
        }

        if (!VIREN_ValidPassword(inputtext))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Пароль должен содержать от 6 до 64 символов.");
            VIREN_ShowRegister(playerid);
            return 1;
        }

        format(VIREN_RegisterPassword[playerid], 65, "%s", inputtext);
        ShowPlayerDialog(
            playerid,
            VIREN_DIALOG_REGISTER_CONFIRM,
            DIALOG_STYLE_PASSWORD,
            "VIREN RP — Подтверждение",
            "Повторите пароль:",
            "Создать",
            "Назад"
        );
        return 1;
    }

    if (dialogid == VIREN_DIALOG_REGISTER_CONFIRM)
    {
        if (!response)
        {
            VIREN_ShowRegister(playerid);
            return 1;
        }

        if (strcmp(VIREN_RegisterPassword[playerid], inputtext, false) != 0)
        {
            SendClientMessage(playerid, 0xFF6666FF, "Пароли не совпадают.");
            VIREN_ShowRegister(playerid);
            return 1;
        }

        VIREN_CreateAccount(playerid, inputtext);
        VIREN_ShowCharacterCreation(playerid);
        return 1;
    }

    if (dialogid == VIREN_DIALOG_LOGIN)
    {
        if (!response)
        {
            Kick(playerid);
            return 1;
        }

        if (VIREN_CheckPassword(playerid, inputtext))
        {
            VIREN_LoginAccount(playerid);
            return 1;
        }

        VIREN_Player[playerid][VIREN_LoginAttempts]++;

        if (VIREN_Player[playerid][VIREN_LoginAttempts] >= 3)
        {
            SendClientMessage(playerid, 0xFF6666FF, "Слишком много неверных попыток.");
            Kick(playerid);
            return 1;
        }

        SendClientMessage(playerid, 0xFF6666FF, "Неверный пароль.");
        VIREN_ShowLogin(playerid);
        return 1;
    }

    if (dialogid == VIREN_DIALOG_CHARACTER_NAME)
    {
        if (!response)
        {
            Kick(playerid);
            return 1;
        }

        if (!VIREN_IsValidCharacterName(inputtext))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Неверное имя. Используйте формат Name_Surname, например John_Smith.");
            VIREN_ShowCharacterCreation(playerid);
            return 1;
        }

        format(VIREN_Player[playerid][VIREN_CharacterName], 32, "%s", inputtext);
        VIREN_ShowGender(playerid);
        return 1;
    }

    if (dialogid == VIREN_DIALOG_CHARACTER_GENDER)
    {
        if (!response)
        {
            VIREN_ShowCharacterCreation(playerid);
            return 1;
        }

        if (listitem < 0 || listitem > 1)
        {
            VIREN_ShowGender(playerid);
            return 1;
        }

        VIREN_Player[playerid][VIREN_Gender] = listitem;
        VIREN_ShowSkin(playerid);
        return 1;
    }

    if (dialogid == VIREN_DIALOG_CHARACTER_SKIN)
    {
        if (!response)
        {
            VIREN_ShowGender(playerid);
            return 1;
        }

        if (listitem < 0 || listitem > 3)
        {
            VIREN_ShowSkin(playerid);
            return 1;
        }

        if (VIREN_Player[playerid][VIREN_Gender] == 0)
        {
            VIREN_Player[playerid][VIREN_Skin] = listitem == 0 ? 26 : 7;
        }
        else
        {
            VIREN_Player[playerid][VIREN_Skin] = listitem == 2 ? 12 : 41;
        }

        VIREN_Player[playerid][VIREN_HasCharacter] = true;
        VIREN_WriteAccount(playerid);
        VIREN_SpawnCharacter(playerid);
        return 1;
    }

    return 0;
}

stock VIREN_PlayerCommand(playerid, const cmdtext[])
{
    if (!VIREN_Player[playerid][VIREN_LoggedIn] ||
        !VIREN_Player[playerid][VIREN_HasCharacter])
    {
        SendClientMessage(playerid, 0xFF6666FF, "Доступно только после авторизации и создания персонажа.");
        return 1;
    }

    if (VIREN_EconomyCommand(playerid, cmdtext))
        return 1;

    if (VIREN_JobCommand(playerid, cmdtext))
        return 1;

    if (VIREN_RPCommand(playerid, cmdtext))
        return 1;

    if (!strcmp(cmdtext, "/stats", true))
    {
        VIREN_ShowStats(playerid);
        return 1;
    }

    if (!strcmp(cmdtext, "/help", true))
    {
        SendClientMessage(playerid, 0x66CCFFFF, "VIREN RP — команды:");
        SendClientMessage(playerid, 0xFFFFFFFF, "/stats /money /pay /deposit /withdraw");
        SendClientMessage(playerid, 0xFFFFFFFF, "/me /do /try /help");
        return 1;
    }

    return 0;
}
