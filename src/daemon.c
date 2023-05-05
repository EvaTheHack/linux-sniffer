#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>

#include "sniffer.c"

#define MSGSZ 1024

typedef struct message
{
    long mtype;
    char mtext[MSGSZ];
} message;

pthread_t thread;

int StartWith(const char *str, const char *substr);
void PrintHelp();
void ChooseAction(char **commands, int msgid);
char **SplitMessage(char message[128]);
void SendMessage(char *message, int msgid, int type);

int main()
{
    key_t key = 12345;
    int msgid;
    message msg;

    if ((msgid = msgget(key, 0644 | IPC_CREAT)) == -1)
    {
        perror("msgget");
        exit(1);
    }

    while (1)
    {
        if (msgrcv(msgid, &msg, MSGSZ, 0, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

        char **commands = (char **)malloc(10 * sizeof(char *));
        commands = SplitMessage(msg.mtext);

        ChooseAction(commands, msgid);
    }

    return 0;
}

void ChooseAction(char **commands, int msgid)
{
    if (strcmp(commands[0], "start") == 0)
    {
        char *iface = ReadConfig();
        pthread_create(&thread, NULL, Start, (void *)iface);
        SendMessage("Sniffer started", msgid, 1);
        return;
    }
    if (strcmp(commands[0], "stop") == 0)
    {
        StopSniffing();
        SendMessage("Sniffer stopped", msgid, 1);
        return;
    }
    if (strcmp(commands[0], "show") == 0)
    {
        char *ip = commands[1];

        char *iface = ReadConfig();
        ListIp list = ReadFromResult(iface);

        int id = IfExistGetId(list, ip);
        if (id == -1)
        {
            SendMessage("This is ip does not exist in list", msgid, 2);
        }
        else
        {
            char buffer[100];
            Ip ip = list.ips[id];
            sprintf(buffer, "IP: %s; Count:%d\n", ip.address, ip.count);

            SendMessage(buffer, msgid, 1);
        }
        return;
    }
    if (strcmp(commands[0], "select") == 0)
    {
        if (IsRunning)
        {
            SendMessage("You can not change iface while it works", msgid, 2);
            return;
        }

        char *iface = commands[2];

        char *eth = malloc(sizeof(char) * 5);
        strcpy(eth, "eth");
        char *wlan = malloc(sizeof(char) * 5);
        strcpy(wlan, "wlan");

        if (!StartWith(iface, eth) || !StartWith(iface, wlan))
        {
            SendMessage("Allowed only eth and wlan ifaces", msgid, 2);
            return;
        }
        Ifaces ifaces = GetAvailableIfaces();
        char option;

        for (int i = 0; i < ifaces.count; i++)
        {
            if (strcmp(ifaces.ifaces[i], iface) == 0)
            {
                WriteToConfig(ifaces.ifaces[i]);
                SendMessage("Config has been setuped\n", msgid, 1);
                return;
            }
        }
        SendMessage("This config is unavailable", msgid, 2);
    }
    if (strcmp(commands[0], "stat") == 0)
    {
        char buffer[MSGSZ];
        buffer[0] = '\0';

        if (commands[1] == NULL)
        {
            ListIp list = GetAllFromResults();
            for (int i = 0; i < list.count; i++)
            {
                char formatted[30];
                Ip ip = list.ips[i];
                sprintf(formatted, "IP: %s; Count:%d\n", ip.address, ip.count);
                strcat(buffer, formatted);
            }
            SendMessage(buffer, msgid, 1);
            return;
        }

        char *iface = commands[1];
        if (!CheckIfFileExist(iface))
        {
            SendMessage("Make sure, that file for this iface exist", msgid, 2);
            return;
        }

        ListIp list = ReadFromResult(iface);
        for (int i = 0; i < list.count; i++)
        {
            char formatted[30];
            Ip ip = list.ips[i];
            sprintf(formatted, "IP: %s; Count:%d\n", ip.address, ip.count);
            strcat(buffer, formatted);
        }
        SendMessage(buffer, msgid, 1);
    }
}

/// type
/// 1 - INFO
/// 2 - ERROR
void SendMessage(char *msg, int msgid, int type)
{
    message message_buf;
    size_t buf_length;

    memset(&message_buf, 0, sizeof(message_buf));
    message_buf.mtype = type;
    strncpy(message_buf.mtext, msg, MSGSZ);

    buf_length = strlen(message_buf.mtext) + 1;

    if (msgsnd(msgid, &message_buf, buf_length, IPC_NOWAIT) < 0)
    {
        perror("msgsnd");
        return;
    }
}

char **SplitMessage(char message[128])
{
    char *token = strtok(message, " ");
    char **commands = (char **)malloc(10 * sizeof(char *));
    int i = 0;
    while (token != NULL)
    {
        commands[i] = token;
        i++;
        // Отримуємо наступний токен
        token = strtok(NULL, " ");
    }
    return commands;
}

int StartWith(const char *str, const char *substr)
{
    int lenstr = strlen(str);
    int lensubstr = strlen(substr);

    if (lensubstr > lenstr)
    {
        return 0;
    }

    return strncmp(substr, str, lensubstr) == 0;
}