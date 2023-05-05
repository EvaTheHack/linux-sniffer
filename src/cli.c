#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSGSZ 1024

void PrintError(char *error);
void PrintInfo(char *message);
void PrintHelp();

typedef struct msgbuf
{
    long mtype;
    char mtext[MSGSZ];
} message;

int main(int argc, char **argv)
{
    int msgid;
    key_t key = 12345;

    message message_snd;
    message message_get;
    size_t buf_length;

    if ((msgid = msgget(key, 0666)) < 0)
    {
        perror("msgget");
        return 1;
    }
    


    if (argc == 1 || strcmp(argv[1], "--help") == 0)
    {
        PrintHelp();
        return 1;
    }

    memset(&message_snd, 0, sizeof(message_snd));
    message_snd.mtype = 1;
    for (int i = 1; i < argc; i++)
    {
        strcat(message_snd.mtext, argv[i]);
        strcat(message_snd.mtext, " ");
    }

    buf_length = strlen(message_snd.mtext) + 1;
    msgsnd(msgid, &message_snd, buf_length, IPC_NOWAIT);

    msgrcv(msgid, &message_get, MSGSZ, 0, 0);

    if(strcmp(message_snd.mtext, message_get.mtext) == 0)
    {
        PrintError("Daemon did not receive request. Try to run on daemon.");
        return 0;
    }

    if (message_get.mtype == 1)
    {
        PrintInfo(message_get.mtext);
    }
    else
    {
        PrintError(message_get.mtext);
    }
    return 0;
}

void PrintHelp()
{
    printf("start - start sniffing \n");
    printf("stop  - stop sniffing \n");
    printf("show [ip] count - print number of packets received from ip address\n");
    printf("select iface [iface] - select iface for sniffing eth0, wlan0, ethN, wlanN\n");
    printf("stat [iface] - show all collected stat for interface, if iface omitted - for all interfaces. \n");
    printf("--help - show usage information\n");
}

void PrintInfo(char *message)
{
    printf("\033[0;32m");
    printf("%s\n", message);
    printf("\033[0m");
}

void PrintError(char *error)
{
    printf("\033[0;31m");
    printf("%s\n", error);
    printf("\033[0m");
}