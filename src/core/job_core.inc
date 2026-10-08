#if defined _VIREN_JOB_CORE_INCLUDED
    #endinput
#endif
#define _VIREN_JOB_CORE_INCLUDED

#define VIREN_JOB_NONE 0
#define VIREN_JOB_COURIER 1
#define VIREN_JOB_COOLDOWN_SECONDS 30
#define VIREN_JOB_WORK_RADIUS 8.0
#define VIREN_JOB_WORK_X 1481.0
#define VIREN_JOB_WORK_Y -1771.0
#define VIREN_JOB_WORK_Z 18.8

stock VIREN_JobName(jobid, output[], size)
{
    switch (jobid)
    {
        case VIREN_JOB_COURIER: format(output, size, "Курьер");
        default: format(output, size, "Безработный");
    }
    return 1;
}

stock VIREN_JobCommand(playerid, const cmdtext[])
{
    if (!strcmp(cmdtext, "/job", true))
    {
        new job[32];
        VIREN_JobName(VIREN_Player[playerid][VIREN_Job], job, sizeof(job));
        new message[96];
        format(message, sizeof(message), "Ваша работа: %s", job);
        SendClientMessage(playerid, 0xFFFFFFFF, message);
        SendClientMessage(playerid, 0x66CCFFFF, "Доступна работа: Курьер. Используйте /job courier для устройства.");
        return 1;
    }

    if (!strcmp(cmdtext, "/job courier", true))
    {
        VIREN_Player[playerid][VIREN_Job] = VIREN_JOB_COURIER;
        VIREN_Player[playerid][VIREN_JobCooldown] = 0;
        VIREN_WriteAccount(playerid);
        SendClientMessage(playerid, 0x66FF66FF, "Вы устроились курьером. Рабочая точка отмечена сообщением: /work.");
        return 1;
    }

    if (!strcmp(cmdtext, "/work", true))
    {
        if (VIREN_Player[playerid][VIREN_Job] != VIREN_JOB_COURIER)
        {
            SendClientMessage(playerid, 0xFF6666FF, "У вас нет работы. Используйте /job.");
            return 1;
        }

        new now = GetTickCount();
        if (now < VIREN_Player[playerid][VIREN_JobCooldown])
        {
            new remaining = (VIREN_Player[playerid][VIREN_JobCooldown] - now) / 1000 + 1;
            new message[96];
            format(message, sizeof(message), "Рабочее действие пока недоступно. Подождите %d сек.", remaining);
            SendClientMessage(playerid, 0xFFCC66FF, message);
            return 1;
        }

        if (GetPlayerDistanceFromPoint(playerid, VIREN_JOB_WORK_X, VIREN_JOB_WORK_Y, VIREN_JOB_WORK_Z) > VIREN_JOB_WORK_RADIUS)
        {
            SendClientMessage(playerid, 0xFF6666FF, "Вы должны находиться на рабочей точке курьера.");
            return 1;
        }

        if (!VIREN_AddMoney(playerid, 750, "courier_work"))
        {
            SendClientMessage(playerid, 0xFF6666FF, "Не удалось начислить зарплату.");
            return 1;
        }

        VIREN_AwardXP(playerid, 25, "courier_work");
        VIREN_Player[playerid][VIREN_JobCooldown] = now + VIREN_JOB_COOLDOWN_SECONDS * 1000;

        SendClientMessage(playerid, 0x66FF66FF, "Работа выполнена: +$750 и +25 XP.");
        return 1;
    }

    return 0;
}
