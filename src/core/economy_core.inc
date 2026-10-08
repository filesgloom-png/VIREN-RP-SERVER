#if defined _VIREN_ECONOMY_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_ECONOMY_CORE_INCLUDED

#define VIREN_MAX_MONEY 2000000000
#define VIREN_MAX_LEVEL 100

stock VIREN_EconomyLog(const action[], playerid, amount, const details[])
{
    new name[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, name, sizeof(name));
    printf("[VIREN][ECONOMY] %s | player=%d name=%s amount=%d | %s", action, playerid, name, amount, details);
    return 1;
}

stock bool:VIREN_EconomyCanAdd(current, amount)
{
    if (amount <= 0)
        return false;

    return current <= VIREN_MAX_MONEY - amount;
}

stock bool:VIREN_EconomyCanTake(current, amount)
{
    return amount > 0 && current >= amount;
}

stock bool:VIREN_AddMoney(playerid, amount, const reason[])
{
    if (!VIREN_EconomyCanAdd(VIREN_Player[playerid][VIREN_Money], amount))
        return false;

    VIREN_Player[playerid][VIREN_Money] += amount;
    GivePlayerMoney(playerid, amount);
    VIREN_WriteAccount(playerid);
    VIREN_HudUpdate(playerid);
    VIREN_EconomyLog("ADD_CASH", playerid, amount, reason);
    return true;
}

stock bool:VIREN_TakeMoney(playerid, amount, const reason[])
{
    if (!VIREN_EconomyCanTake(VIREN_Player[playerid][VIREN_Money], amount))
        return false;

    VIREN_Player[playerid][VIREN_Money] -= amount;
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, VIREN_Player[playerid][VIREN_Money]);
    VIREN_WriteAccount(playerid);
    VIREN_HudUpdate(playerid);
    VIREN_EconomyLog("TAKE_CASH", playerid, amount, reason);
    return true;
}

stock bool:VIREN_AddBank(playerid, amount, const reason[])
{
    if (!VIREN_EconomyCanAdd(VIREN_Player[playerid][VIREN_Bank], amount))
        return false;

    VIREN_Player[playerid][VIREN_Bank] += amount;
    VIREN_WriteAccount(playerid);
    VIREN_HudUpdate(playerid);
    VIREN_EconomyLog("ADD_BANK", playerid, amount, reason);
    return true;
}

stock bool:VIREN_TakeBank(playerid, amount, const reason[])
{
    if (!VIREN_EconomyCanTake(VIREN_Player[playerid][VIREN_Bank], amount))
        return false;

    VIREN_Player[playerid][VIREN_Bank] -= amount;
    VIREN_WriteAccount(playerid);
    VIREN_HudUpdate(playerid);
    VIREN_EconomyLog("TAKE_BANK", playerid, amount, reason);
    return true;
}

stock VIREN_EconomySyncCash(playerid)
{
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, VIREN_Player[playerid][VIREN_Money]);
    return 1;
}

stock VIREN_AwardXP(playerid, amount, const reason[])
{
    new level = VIREN_Player[playerid][VIREN_Level];
    new xp = VIREN_Player[playerid][VIREN_XP];

    if (amount <= 0 || level >= VIREN_MAX_LEVEL)
        return 0;

    if (xp > VIREN_MAX_MONEY - amount)
        xp = VIREN_MAX_MONEY;

    else
        xp += amount;

    while (level < VIREN_MAX_LEVEL && xp >= level * 100)
    {
        xp -= level * 100;
        level++;
        SendClientMessage(playerid, 0x66FF66FF, "Поздравляем! Вы получили новый уровень.");
    }

    VIREN_Player[playerid][VIREN_Level] = level;
    VIREN_Player[playerid][VIREN_XP] = xp;
    VIREN_WriteAccount(playerid);
    VIREN_HudUpdate(playerid);
    VIREN_EconomyLog("ADD_XP", playerid, amount, reason);
    return 1;
}

stock bool:VIREN_ParseAmount(const command[], start, &amount)
{
    new length = strlen(command);
    new index = start;
    new buffer[16];
    new count = 0;

    while (index < length && command[index] == ' ')
        index++;

    while (index < length && command[index] >= '0' && command[index] <= '9' && count < sizeof(buffer) - 1)
    {
        buffer[count++] = command[index++];
    }

    buffer[count] = EOS;

    if (count == 0 || index < length && command[index] != ' ')
        return false;

    amount = strval(buffer);
    return amount > 0 && amount <= 1000000000;
}

stock bool:VIREN_ParsePay(const command[], &targetid, &amount)
{
    new length = strlen(command);
    new index = 4;
    new buffer[16];
    new count = 0;

    while (index < length && command[index] == ' ')
        index++;

    while (index < length && command[index] >= '0' && command[index] <= '9' && count < sizeof(buffer) - 1)
    {
        buffer[count++] = command[index++];
    }

    buffer[count] = EOS;

    if (count == 0)
        return false;

    targetid = strval(buffer);

    while (index < length && command[index] == ' ')
        index++;

    count = 0;

    while (index < length && command[index] >= '0' && command[index] <= '9' && count < sizeof(buffer) - 1)
    {
        buffer[count++] = command[index++];
    }

    buffer[count] = EOS;

    if (count == 0)
        return false;

    if (index < length && command[index] != ' ')
        return false;

    amount = strval(buffer);
    return amount > 0 && amount <= 1000000000;
}

stock VIREN_EconomyCommand(playerid, const cmdtext[])
{
    new amount;
    new targetid;
    new details[96];

    if (!strcmp(cmdtext, "/money", true))
    {
        new message[96];
        format(message, sizeof(message), "Наличные: $%d | Банк: $%d", VIREN_Player[playerid][VIREN_Money], VIREN_Player[playerid][VIREN_Bank]);
        SendClientMessage(playerid, 0xFFFFFFFF, message);
        return 1;
    }

    if (!strncmp(cmdtext, "/pay", 4, true))
    {
        if (!VIREN_ParsePay(cmdtext, targetid, amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Использование: /pay [ID] [сумма]");
            return 1;
        }

        if (targetid == playerid || targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Игрок не найден.");
            return 1;
        }

        if (!VIREN_Player[targetid][VIREN_LoggedIn] || !VIREN_Player[targetid][VIREN_HasCharacter])
        {
            SendClientMessage(playerid, 0xFF6666FF, "Этот игрок ещё не вошёл в игру.");
            return 1;
        }

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);

        if (GetPlayerDistanceFromPoint(targetid, x, y, z) > 5.0)
        {
            SendClientMessage(playerid, 0xFF6666FF, "Игрок должен находиться рядом.");
            return 1;
        }

        if (!VIREN_EconomyCanTake(VIREN_Player[playerid][VIREN_Money], amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Недостаточно наличных.");
            return 1;
        }

        if (!VIREN_EconomyCanAdd(VIREN_Player[targetid][VIREN_Money], amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "У получателя слишком большой баланс.");
            return 1;
        }

        VIREN_Player[playerid][VIREN_Money] -= amount;
        VIREN_Player[targetid][VIREN_Money] += amount;
        VIREN_EconomySyncCash(playerid);
        VIREN_EconomySyncCash(targetid);

        VIREN_WriteAccount(playerid);
        VIREN_WriteAccount(targetid);
        VIREN_HudUpdate(playerid);
        VIREN_HudUpdate(targetid);

        format(details, sizeof(details), "transfer_to=%d", targetid);
        VIREN_EconomyLog("PAY_OUT", playerid, amount, details);
        format(details, sizeof(details), "transfer_from=%d", playerid);
        VIREN_EconomyLog("PAY_IN", targetid, amount, details);

        SendClientMessage(playerid, 0x66FF66FF, "Деньги переданы.");
        SendClientMessage(targetid, 0x66FF66FF, "Вы получили деньги.");
        return 1;
    }

    if (!strncmp(cmdtext, "/deposit", 8, true))
    {
        if (!VIREN_ParseAmount(cmdtext, 8, amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Использование: /deposit [сумма]");
            return 1;
        }

        if (!VIREN_EconomyCanTake(VIREN_Player[playerid][VIREN_Money], amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Недостаточно наличных.");
            return 1;
        }

        if (!VIREN_EconomyCanAdd(VIREN_Player[playerid][VIREN_Bank], amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Банковский баланс достиг лимита.");
            return 1;
        }

        VIREN_Player[playerid][VIREN_Money] -= amount;
        VIREN_Player[playerid][VIREN_Bank] += amount;
        VIREN_EconomySyncCash(playerid);
        VIREN_WriteAccount(playerid);
        VIREN_HudUpdate(playerid);

        VIREN_EconomyLog("DEPOSIT", playerid, amount, "cash_to_bank");
        SendClientMessage(playerid, 0x66FF66FF, "Деньги внесены на банковский счёт.");
        return 1;
    }

    if (!strncmp(cmdtext, "/withdraw", 9, true))
    {
        if (!VIREN_ParseAmount(cmdtext, 9, amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Использование: /withdraw [сумма]");
            return 1;
        }

        if (!VIREN_EconomyCanTake(VIREN_Player[playerid][VIREN_Bank], amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Недостаточно денег на банковском счёте.");
            return 1;
        }

        if (!VIREN_EconomyCanAdd(VIREN_Player[playerid][VIREN_Money], amount))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Нельзя получить столько наличных.");
            return 1;
        }

        VIREN_Player[playerid][VIREN_Bank] -= amount;
        VIREN_Player[playerid][VIREN_Money] += amount;
        VIREN_EconomySyncCash(playerid);
        VIREN_WriteAccount(playerid);
        VIREN_HudUpdate(playerid);

        VIREN_EconomyLog("WITHDRAW", playerid, amount, "bank_to_cash");
        SendClientMessage(playerid, 0x66FF66FF, "Деньги сняты с банковского счёта.");
        return 1;
    }

    return 0;
}
