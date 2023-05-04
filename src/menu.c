#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "sniffer.c"

void ShowAvailableIfaces();
void ShowCountByIp();
void StartSniff(pthread_t thread);
void ShowStatisticByIface();
void Stop(pthread_t thread);

void ClearConsole();
void PrintInfo(char *message, char *additional);
void PrintError(char *message);

void StartMenu(pthread_t thread)
{
    char option;
    int age;
    while (1)
    {
        printf("1. Start\n");
        printf("2. Stop\n");
        printf("3. Show count by particular ip\n");
        printf("4. Select iface\n");
        printf("5. Statistic by iface\n");
        printf("6. Exit\n");
        scanf("%c", &option); //sometimes lagging
        getchar();
        switch (option)
        {
        case '1':
            StartSniff(thread);
            break;
        case '2':
            Stop(thread);
            break;
        case '3':
            ShowCountByIp();
            break;
        case '4':
            ShowAvailableIfaces();
            break;
        case '5':
            ShowStatisticByIface();
            break;
        case '6':
            exit(0);
            break;
        default:
            printf("Wrong option\n");
            break;
        }
    }
}

void StartSniff(pthread_t thread)
{
    char *iface = ReadConfig();
    pthread_create(&thread, NULL, Start, (void *)iface);
    PrintInfo("Sniffing started on the\n", iface);
}

void Stop(pthread_t thread)
{
    StopSniffing();
    PrintInfo("Sniffing stopped", "");
}

void ShowCountByIp()
{
    char ip[16];
    printf("Enter ip\n");
    scanf("%s", ip);

    char *iface = ReadConfig();
    ListIp list = ReadFromResult(iface);

    int id = IfExistGetId(list, ip);
    if (id == -1)
    {
        PrintError("This is ip does not exist in list");
    }
    else
    {
        PrintInfo("IP founded\n", "");
        Ip ip = list.ips[id];
        printf("IP: %s; Count:%d\n", ip.address, ip.count);
    }
    printf("\n");
}

void ShowAvailableIfaces()
{
    char iface[10];
    printf("Enter iface\n");
    scanf("%s", iface);

    Ifaces ifaces = GetAvailableIfaces();
    char option;

    for (int i = 0; i < ifaces.count; i++)
    {
        if (strcmp(ifaces.ifaces[i], iface) == 0)
        {
            WriteToConfig(ifaces.ifaces[i]);
            ClearConsole();
            PrintInfo("Config has been setuped\n", "");
            return;
        }
    }
    ClearConsole();
    PrintError("This config is unavailable");
}

void ShowStatisticByIface()
{
    char buff[10];
    printf("Enter iface\n");
    scanf("%s", buff);

    char *iface = buff;

    ClearConsole();
    if(!CheckIfFileExist(iface))
    {
        PrintError("Make sure, that file for this iface exist");
        return;
    }

    PrintInfo("Statistic found\n", "");
    ListIp list = ReadFromResult(iface);
    for (int i = 0; i < list.count; i++)
    {
        Ip ip = list.ips[i];
        printf("IP: %s; Count:%d\n", ip.address, ip.count);
    }
    printf("\n");
}

void PrintInfo(char *message, char *additional)
{
    printf("\033[0;32m");
    printf("%s %s\n", message, additional);
    printf("\033[0m");
}

void PrintError(char *error)
{
    printf("\033[0;31m");
    printf("%s\n", error);
    printf("\033[0m");
}

void ClearConsole()
{
    system("clear");
}