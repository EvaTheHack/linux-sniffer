#include "sniffer.c"

#define MSGSZ 1024

typedef struct message
{
    long mtype;
    char mtext[MSGSZ];
} message;

pthread_t thread;
int message_id;

int CheckIfIfaceExist(char *iface);
int IsIfaceMatchMask(char *iface);
void MapListToBuffer(ListIp list, char *buffer);
int StartWith(const char *str, const char *substr);
void SendMessage(char *msg, int msgid, int type);
int Compare(char *command, char *static_command);

void StartSniff()
{
    char *iface = ReadConfig();
    pthread_create(&thread, NULL, Start, (void *)iface);
    SendMessage("Sniffer started", message_id, 1);
}

void Stop()
{
    StopSniffing();
    SendMessage("Sniffer stopped", message_id, 1);
}

void Show(char *ip)
{
    if(ip == NULL)
    {
        SendMessage("You must provide ip", message_id, 2);
        return;
    }
    char *iface = ReadConfig();
    ListIp list = ReadFromResult(iface);

    int id = IfExistGetId(list, ip);
    if (id == -1)
    {
        SendMessage("This is ip does not exist in list", message_id, 2);
    }
    else
    {
        char buffer[100];
        Ip ip = list.ips[id];
        sprintf(buffer, "IP: %s; Count:%d\n", ip.address, ip.count);

        SendMessage(buffer, message_id, 1);
    }
}

void Select(char *iface)
{
    if (IsRunning)
    {
        SendMessage("You can not change iface while it works", message_id, 2);
        return;
    }

    if(iface == NULL)
    {
        SendMessage("You must provide iface", message_id, 2);
        return;
    }

    int isGoodIface = IsIfaceMatchMask(iface);

    if (!isGoodIface)
    {
        SendMessage("Allowed only eth and wlan ifaces", message_id, 2);
        return;
    }

    int isIfaceExist = CheckIfIfaceExist(iface);

    if(isIfaceExist)
    {
        WriteToConfig(iface);
        SendMessage("Iface has been setuped\n", message_id, 1);
        return;
    }

    SendMessage("This iface is unavailable", message_id, 2);
}

void Stat(char *iface, int count)
{
    char buffer[MSGSZ];
    buffer[0] = '\0';
    if (count == 1)
    {
        ListIp list = GetAllFromResults();
        MapListToBuffer(list, buffer);
        SendMessage(buffer, message_id, 1);
        return;
    }
    
    int isFileExist = CheckIfFileExist(iface);
    if (!isFileExist)
    {
        SendMessage("Make sure, that file for this iface exist", message_id, 2);
        return;
    }

    ListIp list = ReadFromResult(iface);
    MapListToBuffer(list, buffer);
    SendMessage(buffer, message_id, 1);
}

void HandleCommands(char **commands, int msgid, int *count)
{
    message_id = msgid;
    char *command = commands[0];

    if(Compare(command, "start"))
    {
        StartSniff();
        return;
    }
    if(Compare(command, "stop"))
    {
        Stop();
        return;
    }
    if(Compare(command, "show"))
    {
        Show(commands[1]);
        return;
    }
    if(Compare(command, "select") && commands[1] != NULL && Compare(commands[1], "iface"))
    {
        Select(commands[2]);
        return;
    }
    if(Compare(command, "stat"))
    {
        Stat(commands[1], *count);
        return;
    }

    SendMessage("Wrong command. Use --help", message_id, 2);
}

int IsIfaceMatchMask(char *iface)
{
    char *eth = malloc(sizeof(char) * 5);
    strcpy(eth, "eth");
    char *wlan = malloc(sizeof(char) * 5);
    strcpy(wlan, "wlan");

    return StartWith(iface, eth) || StartWith(iface, wlan);
}

int CheckIfIfaceExist(char *iface)
{
    Ifaces ifaces = GetAvailableIfaces();

    for (int i = 0; i < ifaces.count; i++)
    {
        if (strcmp(ifaces.ifaces[i], iface) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void MapListToBuffer(ListIp list, char *buffer)
{
    for (int i = 0; i < list.count; i++)
    {
        char formatted[30];
        Ip ip = list.ips[i];
        sprintf(formatted, "IP: %s; Count:%d\n", ip.address, ip.count);
        strcat(buffer, formatted);
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

int Compare(char *command, char *static_command)
{
    if(strcmp(command, static_command) == 0)
    {
        return 1;
    }

    return 0;
}