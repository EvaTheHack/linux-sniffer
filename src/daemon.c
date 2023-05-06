#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "commands-handler.c"

#define MSGSZ 1024

char **SplitMessage(char message[128], int *count);

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
        int count_commands = 0;
        int *count_ptr = &count_commands;

        char **commands = (char **)malloc(10 * sizeof(char *));
        commands = SplitMessage(msg.mtext, count_ptr);
        HandleCommands(commands, msgid, count_ptr);
    }

    return 0;
}

char **SplitMessage(char message[128], int *count)
{
    char *token = strtok(message, " ");
    char **commands = (char **)malloc(10 * sizeof(char *));
    int i = 0;
    while (token != NULL)
    {
        commands[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    *count = i;
    return commands;
}