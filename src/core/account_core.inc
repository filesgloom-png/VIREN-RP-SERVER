#if defined _VIREN_ACCOUNT_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_ACCOUNT_CORE_INCLUDED

#define VIREN_DIALOG_REGISTER_PASSWORD 100
#define VIREN_DIALOG_REGISTER_CONFIRM 101
#define VIREN_DIALOG_LOGIN 102

new VIREN_RegisterPassword[MAX_PLAYERS][65];

stock VIREN_ShowRegister(playerid)
{
    ShowPlayerDialog(
        playerid,
        VIREN_DIALOG_REGISTER_PASSWORD,
        DIALOG_STYLE_PASSWORD,
        "VIREN RP — Регистрация",
        "Придумайте пароль для аккаунта.\nМинимум 6 символов.",
        "Далее",
        "Выход"
    );
    return 1;
}

stock VIREN_ShowLogin(playerid)
{
    ShowPlayerDialog(
        playerid,
        VIREN_DIALOG_LOGIN,
        DIALOG_STYLE_PASSWORD,
        "VIREN RP — Авторизация",
        "Введите пароль аккаунта:",
        "Войти",
        "Выход"
    );
    return 1;
}

stock bool:VIREN_ValidPassword(const password[])
{
    return strlen(password) >= 6 && strlen(password) <= 64;
}

stock VIREN_GenerateSalt(playerid)
{
    format(
        VIREN_Player[playerid][VIREN_Salt],
        65,
        "%08x%08x%08x",
        GetTickCount(),
        random(2147483647),
        playerid
    );
    return 1;
}

stock VIREN_HashPassword(playerid, const password[])
{
    SHA256_PassHash(
        password,
        VIREN_Player[playerid][VIREN_Salt],
        VIREN_Player[playerid][VIREN_PasswordHash],
        65
    );
    return 1;
}

stock VIREN_CreateAccount(playerid, const password[])
{
    VIREN_GenerateSalt(playerid);
    VIREN_HashPassword(playerid, password);

    VIREN_Player[playerid][VIREN_Money] = 5000;
    VIREN_Player[playerid][VIREN_Bank] = 10000;
    VIREN_Player[playerid][VIREN_Level] = 1;
    VIREN_Player[playerid][VIREN_XP] = 0;
    VIREN_Player[playerid][VIREN_LoggedIn] = true;

    VIREN_WriteAccount(playerid);

    SendClientMessage(playerid, 0x66FF66FF, "Аккаунт создан. Теперь создадим персонажа.");
    return 1;
}

stock bool:VIREN_CheckPassword(playerid, const password[])
{
    new hash[65];

    SHA256_PassHash(password, VIREN_Player[playerid][VIREN_Salt], hash, sizeof(hash));
    return !strcmp(hash, VIREN_Player[playerid][VIREN_PasswordHash], false);
}

stock VIREN_LoginAccount(playerid)
{
    VIREN_LoadAccount(playerid);
    VIREN_Player[playerid][VIREN_LoggedIn] = true;

    SendClientMessage(playerid, 0x66FF66FF, "Авторизация успешна.");

    if (VIREN_Player[playerid][VIREN_HasCharacter])
    {
        VIREN_SpawnCharacter(playerid);
    }
    else
    {
        VIREN_ShowCharacterCreation(playerid);
    }
    return 1;
}
