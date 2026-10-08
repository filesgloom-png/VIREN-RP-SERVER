#if defined _VIREN_ADMIN_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_ADMIN_CORE_INCLUDED

#define VIREN_ADMIN_PLAYER 0
#define VIREN_ADMIN_MODERATOR 1
#define VIREN_ADMIN_ADMIN 2
#define VIREN_ADMIN_SENIOR 3
#define VIREN_ADMIN_OWNER 4

stock VIREN_AdminLevelName(level, output[], size)
{
    switch (level)
    {
        case VIREN_ADMIN_MODERATOR: format(output, size, "Moderator");
        case VIREN_ADMIN_ADMIN: format(output, size, "Admin");
        case VIREN_ADMIN_SENIOR: format(output, size, "Senior Admin");
        case VIREN_ADMIN_OWNER: format(output, size, "Owner");
        default: format(output, size, "Player");
    }
    return 1;
}

stock bool:VIREN_AdminHasLevel(playerid, required)
{
    return VIREN_Player[playerid][VIREN_AdminLevel] >= required;
}

stock VIREN_AdminLog(const action[], actorid, targetid, detail)
{
    printf("[VIREN][ADMIN] action=%s actor=%d target=%d detail=%d", action, actorid, targetid, detail);
    return 1;
}

stock bool:VIREN_AdminParseTarget(const text[], start, &targetid)
{
    new target_text[16];
    strmid(target_text, text, start, strlen(text), sizeof(target_text));

    if (strlen(target_text) == 0)
        return false;

    targetid = strval(target_text);
    return targetid >= 0 && targetid < MAX_PLAYERS && IsPlayerConnected(targetid);
}

stock VIREN_AdminSetFaction(playerid, targetid, factionid, rank)
{
    if (!VIREN_AdminHasLevel(playerid, VIREN_ADMIN_ADMIN))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Недостаточно прав.");
        return 1;
    }

    if (!VIREN_FactionExists(factionid))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Фракция должна быть от 1 до 4.");
        return 1;
    }

    if (rank < VIREN_FACTION_MIN_RANK || rank > VIREN_FactionMaxRank(factionid))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Неверный ранг.");
        return 1;
    }

    VIREN_Player[targetid][VIREN_FactionID] = factionid;
    VIREN_Player[targetid][VIREN_FactionRank] = rank;
    VIREN_WriteAccount(targetid);

    new faction_name[32];
    new message[144];
    VIREN_FactionName(factionid, faction_name, sizeof(faction_name));
    format(message, sizeof(message), "Игрок %d назначен во фракцию %s, ранг %d.", targetid, faction_name, rank);
    SendClientMessage(playerid, 0x66FF66FF, message);
    format(message, sizeof(message), "Администратор назначил вас во фракцию %s, ранг %d.", faction_name, rank);
    SendClientMessage(targetid, 0x66CCFFFF, message);

    VIREN_AdminLog("setfaction", playerid, targetid, factionid * 100 + rank);
    return 1;
}

stock VIREN_AdminSetRank(playerid, targetid, rank)
{
    if (!VIREN_FactionExists(VIREN_Player[targetid][VIREN_FactionID]))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок не состоит во фракции.");
        return 1;
    }

    if (rank < VIREN_FACTION_MIN_RANK || rank > VIREN_FactionMaxRank(VIREN_Player[targetid][VIREN_FactionID]))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Неверный ранг.");
        return 1;
    }

    if (VIREN_AdminHasLevel(playerid, VIREN_ADMIN_ADMIN))
    {
        VIREN_Player[targetid][VIREN_FactionRank] = rank;
        VIREN_WriteAccount(targetid);
        VIREN_AdminLog("setrank", playerid, targetid, rank);
    }
    else
    {
        if (VIREN_Player[playerid][VIREN_FactionID] != VIREN_Player[targetid][VIREN_FactionID] ||
            !VIREN_FactionHasRank(playerid, 8))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Недостаточно прав для управления рангами.");
            return 1;
        }

        if (targetid == playerid ||
            VIREN_Player[targetid][VIREN_FactionRank] >= VIREN_Player[playerid][VIREN_FactionRank] ||
            rank >= VIREN_Player[playerid][VIREN_FactionRank] ||
            rank == VIREN_FactionMaxRank(VIREN_Player[playerid][VIREN_FactionID]))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Вы можете изменять только более низкие ранги.");
            return 1;
        }

        VIREN_Player[targetid][VIREN_FactionRank] = rank;
        VIREN_WriteAccount(targetid);
        VIREN_AdminLog("faction_setrank", playerid, targetid, rank);
    }

    new message[96];
    format(message, sizeof(message), "Игрок %d получил ранг %d.", targetid, rank);
    SendClientMessage(playerid, 0x66FF66FF, message);
    format(message, sizeof(message), "Ваш ранг изменён на %d.", rank);
    SendClientMessage(targetid, 0x66CCFFFF, message);
    return 1;
}

stock VIREN_AdminInvite(playerid, targetid)
{
    if (VIREN_Player[playerid][VIREN_FactionID] == VIREN_FACTION_NONE ||
        !VIREN_FactionHasRank(playerid, 8))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Только руководство фракции может приглашать игроков.");
        return 1;
    }

    if (VIREN_Player[targetid][VIREN_FactionID] != VIREN_FACTION_NONE)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок уже состоит во фракции.");
        return 1;
    }

    VIREN_Player[targetid][VIREN_FactionID] = VIREN_Player[playerid][VIREN_FactionID];
    VIREN_Player[targetid][VIREN_FactionRank] = VIREN_FACTION_MIN_RANK;
    VIREN_WriteAccount(targetid);

    new faction_name[32];
    new message[128];
    VIREN_FactionName(VIREN_Player[playerid][VIREN_FactionID], faction_name, sizeof(faction_name));
    format(message, sizeof(message), "Вы приняли игрока %d во фракцию %s.", targetid, faction_name);
    SendClientMessage(playerid, 0x66FF66FF, message);
    format(message, sizeof(message), "Вас приняли во фракцию %s.", faction_name);
    SendClientMessage(targetid, 0x66CCFFFF, message);

    VIREN_AdminLog("faction_invite", playerid, targetid, VIREN_Player[playerid][VIREN_FactionID]);
    return 1;
}

stock VIREN_AdminUninvite(playerid, targetid)
{
    if (VIREN_Player[playerid][VIREN_FactionID] == VIREN_FACTION_NONE ||
        !VIREN_FactionHasRank(playerid, 8))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Только руководство фракции может исключать игроков.");
        return 1;
    }

    if (VIREN_Player[targetid][VIREN_FactionID] != VIREN_Player[playerid][VIREN_FactionID])
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок не состоит в вашей фракции.");
        return 1;
    }

    if (targetid == playerid ||
        VIREN_Player[targetid][VIREN_FactionRank] >= VIREN_Player[playerid][VIREN_FactionRank])
    {
        SendClientMessage(playerid, 0xFF6666FF, "Нельзя исключить игрока равного или более высокого ранга.");
        return 1;
    }

    VIREN_FactionReset(targetid);
    VIREN_WriteAccount(targetid);
    SendClientMessage(playerid, 0x66FF66FF, "Игрок исключён из фракции.");
    SendClientMessage(targetid, 0xFFCC66FF, "Вы исключены из фракции.");

    VIREN_AdminLog("faction_uninvite", playerid, targetid, 0);
    return 1;
}

stock VIREN_FactionTransferLeader(playerid, targetid)
{
    new factionid = VIREN_Player[playerid][VIREN_FactionID];

    if (factionid == VIREN_FACTION_NONE ||
        !VIREN_FactionIsLeader(playerid))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Только лидер может передавать лидерство.");
        return 1;
    }

    if (targetid == playerid ||
        VIREN_Player[targetid][VIREN_FactionID] != factionid)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Игрок должен состоять в вашей фракции.");
        return 1;
    }

    if (VIREN_Player[targetid][VIREN_FactionRank] >= VIREN_FACTION_MAX_RANK)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Этот игрок уже лидер.");
        return 1;
    }

    VIREN_Player[playerid][VIREN_FactionRank] = VIREN_FACTION_MAX_RANK - 1;
    VIREN_Player[targetid][VIREN_FactionRank] = VIREN_FACTION_MAX_RANK;
    VIREN_WriteAccount(playerid);
    VIREN_WriteAccount(targetid);

    SendClientMessage(playerid, 0x66FF66FF, "Вы передали лидерство.");
    SendClientMessage(targetid, 0x66CCFFFF, "Вы стали лидером фракции.");
    VIREN_AdminLog("faction_transfer_leader", playerid, targetid, factionid);
    return 1;
}

stock VIREN_AdminCommand(playerid, const cmdtext[])
{
    if (!strcmp(cmdtext, "/admins", true))
    {
        new level_name[32];
        new line[128];
        VIREN_AdminLevelName(VIREN_Player[playerid][VIREN_AdminLevel], level_name, sizeof(level_name));
        format(line, sizeof(line), "Ваш Admin Level: %d (%s).", VIREN_Player[playerid][VIREN_AdminLevel], level_name);
        SendClientMessage(playerid, 0x66CCFFFF, line);
        return 1;
    }

    if (strfind(cmdtext, "/setfaction ", true) == 0)
    {
        if (!VIREN_AdminHasLevel(playerid, VIREN_ADMIN_ADMIN))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Недостаточно прав.");
            return 1;
        }

        new args[64];
        new token[16];
        new targetid = -1;
        new factionid = -1;
        new rank = 1;
        new pos = 12;

        strmid(args, cmdtext, pos, strlen(cmdtext), sizeof(args));

        while (args[pos - 12] == ' ') pos++;
        new start = pos;
        while (cmdtext[pos] != EOS && cmdtext[pos] != ' ') pos++;
        strmid(token, cmdtext, start, pos, sizeof(token));
        targetid = strval(token);

        while (cmdtext[pos] == ' ') pos++;
        start = pos;
        while (cmdtext[pos] != EOS && cmdtext[pos] != ' ') pos++;
        strmid(token, cmdtext, start, pos, sizeof(token));
        factionid = strval(token);

        while (cmdtext[pos] == ' ') pos++;
        if (cmdtext[pos] != EOS)
        {
            start = pos;
            while (cmdtext[pos] != EOS && cmdtext[pos] != ' ') pos++;
            strmid(token, cmdtext, start, pos, sizeof(token));
            rank = strval(token);
        }

        if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
            return 1;
        }

        return VIREN_AdminSetFaction(playerid, targetid, factionid, rank);
    }

    if (strfind(cmdtext, "/setrank ", true) == 0)
    {
        new args[48];
        new token[16];
        new targetid = -1;
        new rank = 0;
        new pos = 9;

        strmid(args, cmdtext, pos, strlen(cmdtext), sizeof(args));

        while (cmdtext[pos] == ' ') pos++;
        new start = pos;
        while (cmdtext[pos] != EOS && cmdtext[pos] != ' ') pos++;
        strmid(token, cmdtext, start, pos, sizeof(token));
        targetid = strval(token);

        while (cmdtext[pos] == ' ') pos++;
        start = pos;
        while (cmdtext[pos] != EOS && cmdtext[pos] != ' ') pos++;
        strmid(token, cmdtext, start, pos, sizeof(token));
        rank = strval(token);

        if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
            return 1;
        }

        return VIREN_AdminSetRank(playerid, targetid, rank);
    }

    if (strfind(cmdtext, "/invite ", true) == 0)
    {
        new args[16];
        strmid(args, cmdtext, 8, strlen(cmdtext), sizeof(args));
        new targetid = strval(args);
        if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
            return 1;
        }
        return VIREN_AdminInvite(playerid, targetid);
    }

    if (strfind(cmdtext, "/uninvite ", true) == 0)
    {
        new args[16];
        strmid(args, cmdtext, 10, strlen(cmdtext), sizeof(args));
        new targetid = strval(args);
        if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
            return 1;
        }
        return VIREN_AdminUninvite(playerid, targetid);
    }

    if (strfind(cmdtext, "/faction transfer ", true) == 0)
    {
        new args[16];
        strmid(args, cmdtext, 18, strlen(cmdtext), sizeof(args));
        new targetid = strval(args);
        if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
            return 1;
        }
        return VIREN_FactionTransferLeader(playerid, targetid);
    }

    return 0;
}
