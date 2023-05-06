#include "file.c"
#include "sniffer.c"

int message_id;
int CheckIfIfaceExist(char *iface);
int IsIfaceMatchMask(char *iface);

void Start()
{
    char *iface = ReadConfig();
    pthread_create(&thread, NULL, Start, (void *)iface);
    SendMessage("Sniffer started", msgid, message_id);
}

void Stop()
{
    StopSniffing();
    SendMessage("Sniffer stopped", msgid, message_id);
}

void Show(char *ip)
{
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
}

void Select(char *iface)
{
    if (IsRunning)
    {
        SendMessage("You can not change iface while it works", msgid, 2);
        return;
    }

    int isGoodIface = IsIfaceMatchMask(iface);

    if (!isGoodIface)
    {
        SendMessage("Allowed only eth and wlan ifaces", msgid, 2);
        return;
    }

    int isIfaceExist = CheckIfIfaceExist(iface);

    if(isIfaceExist)
    {
        WriteToConfig(iface);
        SendMessage("Iface has been setuped\n", msgid, 1);
        return;
    }

    SendMessage("This iface is unavailable", msgid, 2);
}

void Stat(char *iface)
{
    char buffer[MSGSZ];
    buffer[0] = '\0';

    int isGoodIface = IsIfaceMatchMask(iface);
    int isIfaceExist = CheckIfIfaceExist(iface);
    if (iface == NULL || isGoodIface || isIfaceExist)
    {
        ListIp list = GetAllFromResults();
        MapListToBuffer(list, buffer);
        
        SendMessage(buffer, msgid, 1);
        return;
    }
    
    if (!CheckIfFileExist(iface))
    {
        SendMessage("Make sure, that file for this iface exist", msgid, 2);
        return;
    }

    ListIp list = ReadFromResult(iface);
    MapListToBuffer(list, buffer);
    SendMessage(buffer, msgid, 1);
}

void HandleCommands(char **commands, int msgid)
{
    message_id = msgid;

    char *command_ptr = commands[0];
    int len = strlen(command_ptr); 
   
    char command[len+1]; 
    
    for (int i = 0; i < len; i++) {
        command[i] = *(command_ptr+i);
    }
   
    command[len] = '\0';
    switch(command)
    {
        case "start":
            Start();
            break;
        case "stop":
            Stop();
            break;
        case "show":
            Show(commands[1]);
            break;
        case "select":
            Select(commands[2]);
            break;
        case "stat":
            Stat();
            break;
        
    }
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