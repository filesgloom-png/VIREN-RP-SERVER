#if defined _VIREN_FACTION_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_FACTION_CORE_INCLUDED

#define VIREN_FACTION_NONE 0
#define VIREN_FACTION_LSPD 1
#define VIREN_FACTION_EMS 2
#define VIREN_FACTION_GOVERNMENT 3
#define VIREN_FACTION_GANG 4

#define VIREN_FACTION_MIN_RANK 1
#define VIREN_FACTION_MAX_RANK 10
#define VIREN_FACTION_PERMISSION_CHAT 1
#define VIREN_FACTION_PERMISSION_INVITE 8
#define VIREN_FACTION_PERMISSION_RANK 8
#define VIREN_FACTION_PERMISSION_LEADER 10

#define VIREN_LSPD_RANK_CADET 1
#define VIREN_LSPD_RANK_OFFICER 2
#define VIREN_LSPD_RANK_SENIOR_OFFICER 3
#define VIREN_LSPD_RANK_CORPORAL 4
#define VIREN_LSPD_RANK_SERGEANT 5
#define VIREN_LSPD_RANK_LIEUTENANT 6
#define VIREN_LSPD_RANK_CAPTAIN 7
#define VIREN_LSPD_RANK_COMMANDER 8
#define VIREN_LSPD_RANK_DEPUTY_CHIEF 9
#define VIREN_LSPD_RANK_CHIEF 10

stock VIREN_FactionName(factionid, output[], size)
{
    switch (factionid)
    {
        case VIREN_FACTION_LSPD: format(output, size, "LSPD");
        case VIREN_FACTION_EMS: format(output, size, "EMS");
        case VIREN_FACTION_GOVERNMENT: format(output, size, "Government");
        case VIREN_FACTION_GANG: format(output, size, "Gang");
        default: format(output, size, "Нет");
    }
    return 1;
}

stock VIREN_FactionTypeName(factionid, output[], size)
{
    switch (factionid)
    {
        case VIREN_FACTION_LSPD: format(output, size, "Police");
        case VIREN_FACTION_EMS: format(output, size, "Medical");
        case VIREN_FACTION_GOVERNMENT: format(output, size, "Government");
        case VIREN_FACTION_GANG: format(output, size, "Criminal");
        default: format(output, size, "None");
    }
    return 1;
}

stock bool:VIREN_FactionExists(factionid)
{
    return factionid >= VIREN_FACTION_LSPD && factionid <= VIREN_FACTION_GANG;
}

stock VIREN_FactionMaxRank(factionid)
{
    if (!VIREN_FactionExists(factionid))
        return 0;

    switch (factionid)
    {
        case VIREN_FACTION_LSPD: return 10;
        case VIREN_FACTION_EMS: return 10;
        case VIREN_FACTION_GOVERNMENT: return 10;
        case VIREN_FACTION_GANG: return 10;
    }
    return 0;
}

stock VIREN_FactionRankName(factionid, rank, output[], size)
{
    if (!VIREN_FactionExists(factionid) || rank < VIREN_FACTION_MIN_RANK || rank > VIREN_FactionMaxRank(factionid))
    {
        format(output, size, "Без ранга");
        return 1;
    }

    if (factionid == VIREN_FACTION_LSPD)
    {
        switch (rank)
        {
            case 1: format(output, size, "Cadet");
            case 2: format(output, size, "Officer");
            case 3: format(output, size, "Senior Officer");
            case 4: format(output, size, "Corporal");
            case 5: format(output, size, "Sergeant");
            case 6: format(output, size, "Lieutenant");
            case 7: format(output, size, "Captain");
            case 8: format(output, size, "Commander");
            case 9: format(output, size, "Deputy Chief");
            case 10: format(output, size, "Chief");
        }
        return 1;
    }

    format(output, size, "Rank %d", rank);
    return 1;
}

stock bool:VIREN_FactionHasPermission(playerid, required_rank)
{
    return VIREN_FactionHasRank(playerid, required_rank);
}

stock VIREN_FactionLog(const action[], actorid, targetid, detail)
{
    printf("[VIREN][FACTION] action=%s actor=%d target=%d detail=%d", action, actorid, targetid, detail);
    return 1;
}

stock VIREN_FactionDuty(playerid)
{
    if (VIREN_Player[playerid][VIREN_FactionID] != VIREN_FACTION_LSPD)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Система duty сейчас доступна только для LSPD.");
        return 1;
    }

    VIREN_Player[playerid][VIREN_OnDuty] = !VIREN_Player[playerid][VIREN_OnDuty];

    if (VIREN_Player[playerid][VIREN_OnDuty])
    {
        SendClientMessage(playerid, 0x66FF66FF, "Вы заступили на службу LSPD.");
        VIREN_FactionLog("duty_on", playerid, playerid, VIREN_Player[playerid][VIREN_FactionRank]);
    }
    else
    {
        SendClientMessage(playerid, 0xFFCC66FF, "Вы закончили службу LSPD.");
        VIREN_FactionLog("duty_off", playerid, playerid, VIREN_Player[playerid][VIREN_FactionRank]);
    }
    return 1;
}

stock VIREN_FactionChat(playerid, const text[])
{
    new factionid = VIREN_Player[playerid][VIREN_FactionID];
    if (factionid == VIREN_FACTION_NONE)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Вы не состоите во фракции.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1];
    new rank_name[32];
    new message[192];
    GetPlayerName(playerid, name, sizeof(name));
    VIREN_FactionRankName(factionid, VIREN_Player[playerid][VIREN_FactionRank], rank_name, sizeof(rank_name));
    format(message, sizeof(message), "[F] %s [%s]: %s", name, rank_name, text);

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) &&
            VIREN_Player[i][VIREN_LoggedIn] &&
            VIREN_Player[i][VIREN_HasCharacter] &&
            VIREN_Player[i][VIREN_FactionID] == factionid)
        {
            SendClientMessage(i, 0x66CCFFFF, message);
        }
    }

    VIREN_FactionLog("chat", playerid, playerid, factionid);
    return 1;
}

stock bool:VIREN_FactionIsLeader(playerid)
{
    new factionid = VIREN_Player[playerid][VIREN_FactionID];
    return factionid != VIREN_FACTION_NONE &&
        VIREN_Player[playerid][VIREN_FactionRank] >= VIREN_FactionMaxRank(factionid);
}

stock bool:VIREN_FactionHasRank(playerid, required_rank)
{
    if (required_rank < VIREN_FACTION_MIN_RANK)
        required_rank = VIREN_FACTION_MIN_RANK;

    return VIREN_Player[playerid][VIREN_FactionID] != VIREN_FACTION_NONE &&
        VIREN_Player[playerid][VIREN_FactionRank] >= required_rank;
}

stock VIREN_FactionReset(playerid)
{
    VIREN_Player[playerid][VIREN_FactionID] = VIREN_FACTION_NONE;
    VIREN_Player[playerid][VIREN_FactionRank] = 0;
    return 1;
}

stock VIREN_FactionJoin(playerid, factionid)
{
    if (!VIREN_FactionExists(factionid))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Неизвестная фракция. Доступны ID: 1 LSPD, 2 EMS, 3 Government, 4 Gang.");
        return 1;
    }

    if (VIREN_Player[playerid][VIREN_FactionID] != VIREN_FACTION_NONE)
    {
        SendClientMessage(playerid, 0xFF6666FF, "Вы уже состоите во фракции. Сначала используйте /faction leave.");
        return 1;
    }

    VIREN_Player[playerid][VIREN_FactionID] = factionid;
    VIREN_Player[playerid][VIREN_FactionRank] = VIREN_FACTION_MIN_RANK;
    VIREN_WriteAccount(playerid);

    new faction_name[32];
    VIREN_FactionName(factionid, faction_name, sizeof(faction_name));

    new message[96];
    format(message, sizeof(message), "Вы вступили во фракцию %s. Ваш ранг: %d.", faction_name, VIREN_FACTION_MIN_RANK);
    SendClientMessage(playerid, 0x66FF66FF, message);
    return 1;
}

stock VIREN_FactionLeave(playerid)
{
    if (VIREN_Player[playerid][VIREN_FactionID] == VIREN_FACTION_NONE)
    {
        SendClientMessage(playerid, 0xFFCC66FF, "Вы не состоите во фракции.");
        return 1;
    }

    if (VIREN_FactionIsLeader(playerid))
    {
        SendClientMessage(playerid, 0xFF6666FF, "Лидер не может выйти из фракции без передачи лидерства.");
        return 1;
    }

    VIREN_FactionReset(playerid);
    VIREN_WriteAccount(playerid);
    SendClientMessage(playerid, 0x66FF66FF, "Вы вышли из фракции.");
    return 1;
}

stock VIREN_FactionCommand(playerid, const cmdtext[])
{
    if (!strcmp(cmdtext, "/faction", true))
    {
        new faction_name[32];
        new faction_type[32];
        new rank_name[32];
        new message[160];

        VIREN_FactionName(VIREN_Player[playerid][VIREN_FactionID], faction_name, sizeof(faction_name));
        VIREN_FactionTypeName(VIREN_Player[playerid][VIREN_FactionID], faction_type, sizeof(faction_type));
        VIREN_FactionRankName(VIREN_Player[playerid][VIREN_FactionID], VIREN_Player[playerid][VIREN_FactionRank], rank_name, sizeof(rank_name));

        format(message, sizeof(message), "Фракция: %s | Тип: %s | Ранг: %d (%s) | Лидер: %s",
            faction_name,
            faction_type,
            VIREN_Player[playerid][VIREN_FactionRank],
            rank_name,
            VIREN_FactionIsLeader(playerid) ? "Да" : "Нет");
        SendClientMessage(playerid, 0xFFFFFFFF, message);
        SendClientMessage(playerid, 0x66CCFFFF, "Команды: /members, /faction leave, /faction transfer [playerid].");
        return 1;
    }

    if (!strcmp(cmdtext, "/members", true))
    {
        new factionid = VIREN_Player[playerid][VIREN_FactionID];
        if (factionid == VIREN_FACTION_NONE)
        {
            SendClientMessage(playerid, 0xFF6666FF, "Вы не состоите во фракции.");
            return 1;
        }

        new faction_name[32];
        new line[144];
        new name[MAX_PLAYER_NAME + 1];
        new count = 0;

        VIREN_FactionName(factionid, faction_name, sizeof(faction_name));
        format(line, sizeof(line), "Участники %s:", faction_name);
        SendClientMessage(playerid, 0x66CCFFFF, line);

        for (new i = 0; i < MAX_PLAYERS; i++)
        {
            if (IsPlayerConnected(i) &&
                VIREN_Player[i][VIREN_LoggedIn] &&
                VIREN_Player[i][VIREN_HasCharacter] &&
                VIREN_Player[i][VIREN_FactionID] == factionid)
            {
                GetPlayerName(i, name, sizeof(name));
                format(line, sizeof(line), "- %s | ранг %d%s",
                    name,
                    VIREN_Player[i][VIREN_FactionRank],
                    VIREN_FactionIsLeader(i) ? " | лидер" : "");
                SendClientMessage(playerid, 0xFFFFFFFF, line);
                count++;
            }
        }

        format(line, sizeof(line), "Онлайн участников: %d.", count);
        SendClientMessage(playerid, 0xFFFFFFFF, line);
        return 1;
    }

    if (!strcmp(cmdtext, "/f", true))
    {
        SendClientMessage(playerid, 0x66CCFFFF, "Использование: /f [сообщение].");
        return 1;
    }

    if (strfind(cmdtext, "/f ", true) == 0)
    {
        new text[144];
        strmid(text, cmdtext, 3, strlen(cmdtext), sizeof(text));
        if (strlen(text) == 0)
        {
            SendClientMessage(playerid, 0xFFCC66FF, "Использование: /f [сообщение].");
            return 1;
        }
        return VIREN_FactionChat(playerid, text);
    }

    if (!strcmp(cmdtext, "/duty", true))
    {
        return VIREN_FactionDuty(playerid);
    }

    if (!strcmp(cmdtext, "/faction leave", true))
    {
        return VIREN_FactionLeave(playerid);
    }

    return 0;
}
