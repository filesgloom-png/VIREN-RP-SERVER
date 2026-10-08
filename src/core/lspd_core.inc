#if defined _VIREN_LSPD_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_LSPD_CORE_INCLUDED

#define VIREN_LSPD_MAX_WANTED 6
#define VIREN_LSPD_MIN_ARREST_RANK 2
#define VIREN_LSPD_MIN_TICKET_RANK 2
#define VIREN_LSPD_MIN_CUFF_RANK 1
#define VIREN_LSPD_MIN_FRISK_RANK 2
#define VIREN_LSPD_MIN_TAKEWEAPON_RANK 4

#define VIREN_LSPD_JAIL_X 1544.0
#define VIREN_LSPD_JAIL_Y -1675.0
#define VIREN_LSPD_JAIL_Z 13.5
#define VIREN_LSPD_JAIL_A 90.0

new VIREN_LSPD_LastCrime[MAX_PLAYERS][96];
new VIREN_LSPD_JailTimer[MAX_PLAYERS];

stock bool:VIREN_LSPD_IsOfficer(playerid)
{
    return VIREN_Player[playerid][VIREN_FactionID] == VIREN_FACTION_LSPD;
}

stock bool:VIREN_LSPD_IsOnDuty(playerid)
{
    return VIREN_LSPD_IsOfficer(playerid) &&
        VIREN_Player[playerid][VIREN_OnDuty];
}

stock bool:VIREN_LSPD_HasRank(playerid, required_rank)
{
    return VIREN_LSPD_IsOnDuty(playerid) &&
        VIREN_Player[playerid][VIREN_FactionRank] >= required_rank;
}

stock VIREN_LSPD_SetUniform(playerid)
{
    if (!VIREN_LSPD_IsOfficer(playerid))
        return 0;

    if (VIREN_Player[playerid][VIREN_Gender] == 0)
        SetPlayerSkin(playerid, 280);
    else
        SetPlayerSkin(playerid, 281);

    return 1;
}

stock VIREN_LSPD_RestoreSkin(playerid)
{
    SetPlayerSkin(playerid, VIREN_Player[playerid][VIREN_Skin]);
    return 1;
}

stock VIREN_LSPD_GiveServiceWeapons(playerid)
{
    ResetPlayerWeapons(playerid);
    GivePlayerWeapon(playerid, 3, 50);
    GivePlayerWeapon(playerid, 24, 70);
    GivePlayerWeapon(playerid, 25, 30);
    return 1;
}

stock VIREN_LSPD_ClearServiceWeapons(playerid)
{
    ResetPlayerWeapons(playerid);
    return 1;
}

stock VIREN_LSPD_Duty(playerid)
{
    if (!VIREN_LSPD_IsOfficer(playerid))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Вы не состоите в LSPD.");
        return 1;
    }

    if (VIREN_Player[playerid][VIREN_OnDuty])
    {
        VIREN_Player[playerid][VIREN_OnDuty] = false;
        VIREN_LSPD_ClearServiceWeapons(playerid);
        VIREN_LSPD_RestoreSkin(playerid);
        VIREN_FactionLog("duty_off", playerid, playerid, VIREN_Player[playerid][VIREN_FactionRank]);
        SendClientMessage(playerid, 0xFFCC66FF, "Вы закончили службу LSPD.");
        return 1;
    }

    VIREN_Player[playerid][VIREN_OnDuty] = true;
    VIREN_LSPD_SetUniform(playerid);
    VIREN_LSPD_GiveServiceWeapons(playerid);
    VIREN_FactionLog("duty_on", playerid, playerid, VIREN_Player[playerid][VIREN_FactionRank]);
    SendClientMessage(playerid, 0x66FF66FF, "Вы заступили на службу LSPD. Выданы форма и табельное оружие.");
    return 1;
}

stock VIREN_LSPD_SetWanted(targetid, level, const reason[])
{
    if (level < 0)
        level = 0;

    if (level > VIREN_LSPD_MAX_WANTED)
        level = VIREN_LSPD_MAX_WANTED;

    VIREN_Player[targetid][VIREN_WantedLevel] = level;
    format(VIREN_LSPD_LastCrime[targetid], sizeof(VIREN_LSPD_LastCrime[]), "%s", reason);

    new message[160];
    if (level == 0)
    {
        SendClientMessage(targetid, 0x66FF66FF, "Ваш уровень розыска снят.");
    }
    else
    {
        format(message, sizeof(message), "Вам установлен розыск: %d/6. Причина: %s", level, reason);
        SendClientMessage(targetid, 0xFF6666FF, message);
    }
    return 1;
}

stock VIREN_LSPD_GetTargetFromCommand(const cmdtext[], start, &targetid, reason[], reason_size)
{
    new pos = start;
    new token[16];
    new token_start;

    while (cmdtext[pos] == ' ')
        pos++;

    if (cmdtext[pos] == EOS)
        return 0;

    token_start = pos;
    while (cmdtext[pos] != EOS && cmdtext[pos] != ' ')
        pos++;

    strmid(token, cmdtext, token_start, pos, sizeof(token));
    targetid = strval(token);

    while (cmdtext[pos] == ' ')
        pos++;

    if (cmdtext[pos] == EOS)
        reason[0] = EOS;
    else
        strmid(reason, cmdtext, pos, strlen(cmdtext), reason_size);

    return 1;
}

stock bool:VIREN_LSPD_IsNear(playerid, targetid, Float:distance)
{
    new Float:px, Float:py, Float:pz;
    new Float:tx, Float:ty, Float:tz;
    GetPlayerPos(playerid, px, py, pz);
    GetPlayerPos(targetid, tx, ty, tz);
    return GetPlayerDistanceFromPoint(targetid, px, py, pz) <= distance;
}

stock bool:VIREN_LSPD_ValidTarget(playerid, targetid)
{
    if (targetid < 0 || targetid >= MAX_PLAYERS ||
        !IsPlayerConnected(targetid) ||
        !VIREN_Player[targetid][VIREN_LoggedIn] ||
        !VIREN_Player[targetid][VIREN_HasCharacter])
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден или не авторизован.");
        return false;
    }

    if (targetid == playerid)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Нельзя применить это действие к себе.");
        return false;
    }

    if (!VIREN_LSPD_IsNear(playerid, targetid, 5.0))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок слишком далеко.");
        return false;
    }

    return true;
}

stock VIREN_LSPD_CommandWanted(playerid, const cmdtext[])
{
    new targetid = playerid;
    new reason[96];

    if (strlen(cmdtext) > 8)
    {
        if (!VIREN_LSPD_GetTargetFromCommand(cmdtext, 8, targetid, reason, sizeof(reason)))
            targetid = playerid;
    }

    if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1];
    new message[160];
    new crime[96];

    GetPlayerName(targetid, name, sizeof(name));
    format(crime, sizeof(crime), "%s", VIREN_LSPD_LastCrime[targetid]);
    if (crime[0] == EOS)
        format(crime, sizeof(crime), "Нет данных");

    format(message, sizeof(message), "Розыск %s: %d/6 | Причина: %s",
        name, VIREN_Player[targetid][VIREN_WantedLevel], crime);
    SendClientMessage(playerid, 0xFFCC66FF, message);
    return 1;
}

stock VIREN_LSPD_CommandSu(playerid, const cmdtext[])
{
    if (!VIREN_LSPD_HasRank(playerid, 2))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Для /su требуется ранг Officer+ и duty.");
        return 1;
    }

    new targetid;
    new reason[96];

    if (!VIREN_LSPD_GetTargetFromCommand(cmdtext, 4, targetid, reason, sizeof(reason)) ||
        reason[0] == EOS)
    {
        SendClientMessage(playerid, 0xFFCC66FF, "Использование: /su [playerid] [причина]");
        return 1;
    }

    if (!VIREN_LSPD_ValidTarget(playerid, targetid))
        return 1;

    new wanted = VIREN_Player[targetid][VIREN_WantedLevel] + 1;
    VIREN_LSPD_SetWanted(targetid, wanted, reason);
    VIREN_FactionLog("su", playerid, targetid, wanted);

    SendClientMessage(playerid, 0x66FF66FF, "Розыск успешно установлен.");
    return 1;
}

stock VIREN_LSPD_CommandCuff(playerid, const cmdtext[], bool:uncuff)
{
    if (!VIREN_LSPD_HasRank(playerid, VIREN_LSPD_MIN_CUFF_RANK))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Для этого действия требуется duty LSPD.");
        return 1;
    }

    new targetid;
    new reason[8];

    if (!VIREN_LSPD_GetTargetFromCommand(cmdtext, uncuff ? 7 : 5, targetid, reason, sizeof(reason)))
    {
        SendClientMessage(playerid, 0xFFCC66FF, uncuff ? "Использование: /uncuff [playerid]" : "Использование: /cuff [playerid]");
        return 1;
    }

    if (!VIREN_LSPD_ValidTarget(playerid, targetid))
        return 1;

    if (uncuff)
    {
        VIREN_Player[targetid][VIREN_Cuffed] = false;
        TogglePlayerControllable(targetid, true);
        SendClientMessage(targetid, 0x66FF66FF, "С вас сняли наручники.");
        SendClientMessage(playerid, 0x66FF66FF, "Наручники сняты.");
        VIREN_FactionLog("uncuff", playerid, targetid, 0);
    }
    else
    {
        VIREN_Player[targetid][VIREN_Cuffed] = true;
        TogglePlayerControllable(targetid, false);
        SendClientMessage(targetid, 0xFFCC66FF, "На вас надели наручники.");
        SendClientMessage(playerid, 0x66FF66FF, "Игрок закован.");
        VIREN_FactionLog("cuff", playerid, targetid, 0);
    }
    return 1;
}

stock VIREN_LSPD_CommandFrisk(playerid, const cmdtext[])
{
    if (!VIREN_LSPD_HasRank(playerid, VIREN_LSPD_MIN_FRISK_RANK))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Для /frisk требуется Officer+ и duty.");
        return 1;
    }

    new targetid;
    new reason[8];

    if (!VIREN_LSPD_GetTargetFromCommand(cmdtext, 7, targetid, reason, sizeof(reason)))
    {
        SendClientMessage(playerid, 0xFFCC66FF, "Использование: /frisk [playerid]");
        return 1;
    }

    if (!VIREN_LSPD_ValidTarget(playerid, targetid))
        return 1;

    new message[192];
    format(message, sizeof(message), "Осмотр: ID %d | Оружие в руках: %d | Розыск: %d/6",
        targetid, GetPlayerWeapon(targetid), VIREN_Player[targetid][VIREN_WantedLevel]);
    SendClientMessage(playerid, 0x66CCFFFF, message);
    VIREN_FactionLog("frisk", playerid, targetid, GetPlayerWeapon(targetid));
    return 1;
}

stock VIREN_LSPD_CommandTakeWeapon(playerid, const cmdtext[])
{
    if (!VIREN_LSPD_HasRank(playerid, VIREN_LSPD_MIN_TAKEWEAPON_RANK))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Для /takeweapon требуется Corporal+ и duty.");
        return 1;
    }

    new targetid;
    new reason[8];

    if (!VIREN_LSPD_GetTargetFromCommand(cmdtext, 12, targetid, reason, sizeof(reason)))
    {
        SendClientMessage(playerid, 0xFFCC66FF, "Использование: /takeweapon [playerid]");
        return 1;
    }

    if (!VIREN_LSPD_ValidTarget(playerid, targetid))
        return 1;

    ResetPlayerWeapons(targetid);
    SendClientMessage(targetid, 0xFFCC66FF, "Ваше оружие изъято сотрудником LSPD.");
    SendClientMessage(playerid, 0x66FF66FF, "Оружие игрока изъято.");
    VIREN_FactionLog("takeweapon", playerid, targetid, 0);
    return 1;
}

stock VIREN_LSPD_CommandTicket(playerid, const cmdtext[])
{
    if (!VIREN_LSPD_HasRank(playerid, VIREN_LSPD_MIN_TICKET_RANK))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Для /ticket требуется Officer+ и duty.");
        return 1;
    }

    new targetid;
    new target_token[16];
    new amount_token[16];
    new pos = 8;
    new reason[64];

    while (cmdtext[pos] == ' ')
        pos++;

    new start = pos;
    while (cmdtext[pos] != EOS && cmdtext[pos] != ' ')
        pos++;

    strmid(target_token, cmdtext, start, pos, sizeof(target_token));
    targetid = strval(target_token);

    while (cmdtext[pos] == ' ')
        pos++;

    start = pos;
    while (cmdtext[pos] != EOS && cmdtext[pos] != ' ')
        pos++;

    while (cmdtext[pos] == ' ')
        pos++;

    start = pos;
    while (cmdtext[pos] != EOS && cmdtext[pos] != ' ')
        pos++;
    strmid(amount_token, cmdtext, start, pos, sizeof(amount_token));
    new fine = strval(amount_token);

    while (cmdtext[pos] == ' ')
        pos++;
    if (cmdtext[pos] != EOS)
        strmid(reason, cmdtext, pos, strlen(cmdtext), sizeof(reason));

    if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) ||
        targetid == playerid)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
        return 1;
    }

    if (fine < 1 || fine > 100000)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Штраф: от $1 до $100000.");
        return 1;
    }

    if (!VIREN_LSPD_IsNear(playerid, targetid, 5.0))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок слишком далеко.");
        return 1;
    }

    if (VIREN_Player[targetid][VIREN_Money] < fine)
    {
        SendClientMessage(playerid, 0xFF6666FF, "У игрока недостаточно наличных.");
        return 1;
    }

    VIREN_Player[targetid][VIREN_Money] -= fine;
    VIREN_Player[playerid][VIREN_Money] += fine;
    GivePlayerMoney(targetid, -fine);
    GivePlayerMoney(playerid, fine);
    VIREN_WriteAccount(targetid);
    VIREN_WriteAccount(playerid);

    new message[160];
    format(message, sizeof(message), "Вы получили штраф $%d. Причина: %s", fine, reason[0] == EOS ? "Нарушение" : reason);
    SendClientMessage(targetid, 0xFFCC66FF, message);
    format(message, sizeof(message), "Вы выписали штраф $%d игроку ID %d.", fine, targetid);
    SendClientMessage(playerid, 0x66FF66FF, message);
    VIREN_FactionLog("ticket", playerid, targetid, fine);
    return 1;
}

stock VIREN_LSPD_CommandArrest(playerid, const cmdtext[])
{
    if (!VIREN_LSPD_HasRank(playerid, VIREN_LSPD_MIN_ARREST_RANK))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Для /arrest требуется Officer+ и duty.");
        return 1;
    }

    new targetid;
    new reason[8];

    if (!VIREN_LSPD_GetTargetFromCommand(cmdtext, 7, targetid, reason, sizeof(reason)))
    {
        SendClientMessage(playerid, 0xFFCC66FF, "Использование: /arrest [playerid]");
        return 1;
    }

    if (!VIREN_LSPD_ValidTarget(playerid, targetid))
        return 1;

    if (!VIREN_Player[targetid][VIREN_Cuffed])
    {
        SendClientMessage(playerid, 0xFF6666FF, "Сначала используйте /cuff.");
        return 1;
    }

    new seconds = 30 + (VIREN_Player[targetid][VIREN_WantedLevel] * 30);
    if (seconds > 240)
        seconds = 240;

    VIREN_Player[targetid][VIREN_JailSeconds] = seconds;
    VIREN_Player[targetid][VIREN_Cuffed] = false;
    TogglePlayerControllable(targetid, true);
    SetPlayerPos(targetid, VIREN_LSPD_JAIL_X, VIREN_LSPD_JAIL_Y, VIREN_LSPD_JAIL_Z);
    SetPlayerFacingAngle(targetid, VIREN_LSPD_JAIL_A);
    SetPlayerInterior(targetid, 6);
    SetPlayerVirtualWorld(targetid, 0);

    if (VIREN_LSPD_JailTimer[targetid] != 0)
        KillTimer(VIREN_LSPD_JailTimer[targetid]);

    VIREN_LSPD_JailTimer[targetid] = SetTimerEx("VIREN_LSPD_JailTick", 1000, true, "d", targetid);
    VIREN_Player[targetid][VIREN_WantedLevel] = 0;
    VIREN_LSPD_LastCrime[targetid][0] = EOS;

    SendClientMessage(targetid, 0xFFCC66FF, "Вы арестованы и отправлены в тюрьму.");
    SendClientMessage(playerid, 0x66FF66FF, "Арест выполнен.");
    VIREN_FactionLog("arrest", playerid, targetid, seconds);
    return 1;
}

forward VIREN_LSPD_JailTick(playerid);
public VIREN_LSPD_JailTick(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (VIREN_Player[playerid][VIREN_JailSeconds] > 0)
    {
        VIREN_Player[playerid][VIREN_JailSeconds]--;

        if (VIREN_Player[playerid][VIREN_JailSeconds] == 0)
        {
            if (VIREN_LSPD_JailTimer[playerid] != 0)
                KillTimer(VIREN_LSPD_JailTimer[playerid]);
            SetPlayerInterior(playerid, 0);
            SetPlayerVirtualWorld(playerid, 0);
            SetPlayerPos(playerid, 1481.0, -1771.0, 18.8);
            SetPlayerFacingAngle(playerid, 90.0);
            VIREN_LSPD_JailTimer[playerid] = 0;
            SendClientMessage(playerid, 0x66FF66FF, "Срок окончен. Вы освобождены.");
            return 0;
        }

        new message[96];
        format(message, sizeof(message), "До освобождения: %d сек.", VIREN_Player[playerid][VIREN_JailSeconds]);
        SendClientMessage(playerid, 0xFFCC66FF, message);
    }

    return 1;
}

stock VIREN_LSPD_OnDisconnect(playerid)
{
    if (VIREN_LSPD_JailTimer[playerid] != 0)
    {
        KillTimer(VIREN_LSPD_JailTimer[playerid]);
        VIREN_LSPD_JailTimer[playerid] = 0;
    }

    VIREN_LSPD_LastCrime[playerid][0] = EOS;
    return 1;
}

stock VIREN_LSPD_OnSpawn(playerid)
{
    if (VIREN_Player[playerid][VIREN_OnDuty])
    {
        VIREN_LSPD_SetUniform(playerid);
        VIREN_LSPD_GiveServiceWeapons(playerid);
    }
    return 1;
}

stock VIREN_LSPD_AddServiceVehicles()
{
    AddStaticVehicleEx(596, 1545.0, -1675.0, 13.3, 90.0, 0, 1, 600);
    AddStaticVehicleEx(596, 1545.0, -1679.0, 13.3, 90.0, 0, 1, 600);
    AddStaticVehicleEx(597, 1545.0, -1683.0, 13.3, 90.0, 0, 1, 600);
    AddStaticVehicleEx(598, 1545.0, -1687.0, 13.3, 90.0, 0, 1, 600);
    return 1;
}

stock VIREN_LSPD_Command(playerid, const cmdtext[])
{
    if (!strcmp(cmdtext, "/wanted", true) ||
        strfind(cmdtext, "/wanted ", true) == 0)
        return VIREN_LSPD_CommandWanted(playerid, cmdtext);

    if (strfind(cmdtext, "/su ", true) == 0)
        return VIREN_LSPD_CommandSu(playerid, cmdtext);

    if (strfind(cmdtext, "/cuff ", true) == 0)
        return VIREN_LSPD_CommandCuff(playerid, cmdtext, false);

    if (strfind(cmdtext, "/uncuff ", true) == 0)
        return VIREN_LSPD_CommandCuff(playerid, cmdtext, true);

    if (strfind(cmdtext, "/arrest ", true) == 0)
        return VIREN_LSPD_CommandArrest(playerid, cmdtext);

    if (strfind(cmdtext, "/ticket ", true) == 0)
        return VIREN_LSPD_CommandTicket(playerid, cmdtext);

    if (strfind(cmdtext, "/frisk ", true) == 0)
        return VIREN_LSPD_CommandFrisk(playerid, cmdtext);

    if (strfind(cmdtext, "/takeweapon ", true) == 0)
        return VIREN_LSPD_CommandTakeWeapon(playerid, cmdtext);

    return 0;
}
