#if defined _VIREN_RP_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_RP_CORE_INCLUDED

#define VIREN_RP_DISTANCE 30.0

stock VIREN_RPExtractText(const command[], start, output[], size)
{
    new length = strlen(command);
    new index = start;
    new count = 0;

    while (index < length && command[index] == ' ')
        index++;

    while (index < length && count < size - 1)
        output[count++] = command[index++];

    output[count] = EOS;
    return count > 0;
}

stock VIREN_RPBroadcastNearby(playerid, const message[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    for (new target = 0; target < MAX_PLAYERS; target++)
    {
        if (!IsPlayerConnected(target))
            continue;

        if (!VIREN_Player[target][VIREN_LoggedIn] ||
            !VIREN_Player[target][VIREN_HasCharacter])
            continue;

        if (GetPlayerDistanceFromPoint(target, x, y, z) <= VIREN_RP_DISTANCE)
            SendClientMessage(target, 0xDDDDDDFF, message);
    }
    return 1;
}

stock VIREN_RPCommand(playerid, const cmdtext[])
{
    new text[128];
    new message[192];

    if (!strncmp(cmdtext, "/me", 3, true))
    {
        if (!VIREN_RPExtractText(cmdtext, 3, text, sizeof(text)))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Использование: /me [действие]");
            return 1;
        }

        format(message, sizeof(message), "* %s %s", VIREN_Player[playerid][VIREN_CharacterName], text);
        VIREN_RPBroadcastNearby(playerid, message);
        return 1;
    }

    if (!strncmp(cmdtext, "/do", 3, true))
    {
        if (!VIREN_RPExtractText(cmdtext, 3, text, sizeof(text)))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Использование: /do [описание]");
            return 1;
        }

        format(message, sizeof(message), "* %s (( %s ))", text, VIREN_Player[playerid][VIREN_CharacterName]);
        VIREN_RPBroadcastNearby(playerid, message);
        return 1;
    }

    if (!strncmp(cmdtext, "/try", 4, true))
    {
        if (!VIREN_RPExtractText(cmdtext, 4, text, sizeof(text)))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Использование: /try [действие]");
            return 1;
        }

        if (random(2) == 1)
            format(message, sizeof(message), "* %s пытается: %s — УСПЕШНО", VIREN_Player[playerid][VIREN_CharacterName], text);
        else
            format(message, sizeof(message), "* %s пытается: %s — НЕУДАЧНО", VIREN_Player[playerid][VIREN_CharacterName], text);

        VIREN_RPBroadcastNearby(playerid, message);
        return 1;
    }

    return 0;
}
