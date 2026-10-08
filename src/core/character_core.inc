#if defined _VIREN_CHARACTER_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_CHARACTER_CORE_INCLUDED

#define VIREN_DIALOG_CHARACTER_NAME 110
#define VIREN_DIALOG_CHARACTER_GENDER 111
#define VIREN_DIALOG_CHARACTER_SKIN 112

stock bool:VIREN_IsNameCharValid(character)
{
    return
        (character >= 'A' && character <= 'Z') ||
        (character >= 'a' && character <= 'z');
}

stock bool:VIREN_IsValidCharacterName(const name[])
{
    new length = strlen(name);
    new underscore = -1;
    new i;

    if (length < 5 || length > 31)
        return false;

    for (i = 0; i < length; i++)
    {
        if (name[i] == '_')
        {
            if (underscore != -1)
                return false;

            underscore = i;
            continue;
        }

        if (!VIREN_IsNameCharValid(name[i]))
            return false;
    }

    if (underscore <= 0 || underscore >= length - 1)
        return false;

    if (name[0] >= 'a' && name[0] <= 'z')
        return false;

    if (name[underscore + 1] >= 'a' && name[underscore + 1] <= 'z')
        return false;

    return true;
}

stock VIREN_ShowCharacterCreation(playerid)
{
    ShowPlayerDialog(
        playerid,
        VIREN_DIALOG_CHARACTER_NAME,
        DIALOG_STYLE_INPUT,
        "VIREN RP — Персонаж",
        "Введите имя персонажа в формате Name_Surname.\nПример: John_Smith",
        "Далее",
        "Выход"
    );
    return 1;
}

stock VIREN_ShowGender(playerid)
{
    ShowPlayerDialog(
        playerid,
        VIREN_DIALOG_CHARACTER_GENDER,
        DIALOG_STYLE_LIST,
        "VIREN RP — Пол",
        "Мужской\nЖенский",
        "Выбрать",
        "Назад"
    );
    return 1;
}

stock VIREN_ShowSkin(playerid)
{
    ShowPlayerDialog(
        playerid,
        VIREN_DIALOG_CHARACTER_SKIN,
        DIALOG_STYLE_LIST,
        "VIREN RP — Skin",
        "Мужской — 26\nМужской — 7\nЖенский — 12\nЖенский — 41",
        "Выбрать",
        "Назад"
    );
    return 1;
}

stock VIREN_SpawnCharacter(playerid)
{
    SetPlayerSkin(playerid, VIREN_Player[playerid][VIREN_Skin]);
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, VIREN_Player[playerid][VIREN_Money]);

    SetSpawnInfo(
        playerid,
        NO_TEAM,
        VIREN_Player[playerid][VIREN_Skin],
        1481.0,
        -1771.0,
        18.8,
        90.0,
        WEAPON_FIST,
        0,
        WEAPON_FIST,
        0,
        WEAPON_FIST,
        0
    );

    TogglePlayerSpectating(playerid, false);
    SpawnPlayer(playerid);

    VIREN_HudCreate(playerid);
    VIREN_HudUpdate(playerid);
    VIREN_HudShow(playerid);

    new message[144];
    format(
        message,
        sizeof(message),
        "Добро пожаловать, %s. Уровень: %d | Деньги: $%d | Банк: $%d",
        VIREN_Player[playerid][VIREN_CharacterName],
        VIREN_Player[playerid][VIREN_Level],
        VIREN_Player[playerid][VIREN_Money],
        VIREN_Player[playerid][VIREN_Bank]
    );
    SendClientMessage(playerid, 0x66FF66FF, message);
    return 1;
}

stock VIREN_ShowStats(playerid)
{
    new message[192];
    format(
        message,
        sizeof(message),
        "Имя: %s | Пол: %s | Skin: %d | Уровень: %d | XP: %d",
        VIREN_Player[playerid][VIREN_CharacterName],
        VIREN_Player[playerid][VIREN_Gender] == 0 ? "мужской" : "женский",
        VIREN_Player[playerid][VIREN_Skin],
        VIREN_Player[playerid][VIREN_Level],
        VIREN_Player[playerid][VIREN_XP]
    );
    SendClientMessage(playerid, 0xFFFFFFFF, message);

    format(
        message,
        sizeof(message),
        "Наличные: $%d | Банк: $%d",
        VIREN_Player[playerid][VIREN_Money],
        VIREN_Player[playerid][VIREN_Bank]
    );
    SendClientMessage(playerid, 0xFFFFFFFF, message);

    VIREN_HudUpdate(playerid);
    return 1;
}
