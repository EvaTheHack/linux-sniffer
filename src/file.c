#include "models/ip.c"
#include "models/ifaces.c"
#include "models/list-ip.c"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

FILE *config;
FILE *result;
FILE *logs;

#define MAX_LINE_LENGTH 100

const char *RESULT_FORMAT_OUT = "%s %d\n";
const char *RESULT_FORMAT_IN = "%s %d";

void trim(char *str);

void WriteToConfig(char *iface)
{
    config = fopen("/home/mr_nachtigal/projects/sniff_last/output/config.txt", "w");
    int length = strlen(iface);
    for (int i = 0; i < length; i++)
    {
        putc(iface[i], config);
    }
    fclose(config);
}

char *ReadConfig()
{
    char myString[10];
    config = fopen("/home/mr_nachtigal/projects/sniff_last/output/config.txt", "r");
    if(config == NULL)
    {
        printf("111111\n");
    }
    fgets(myString, 10, config);
    fclose(config);
    char *iface = malloc(10);
    for (int i = 0; i < 10; i++)
    {
        iface[i] = myString[i];
    }
    return iface;
}

void WriteToResult(char *filename, ListIp list)
{
    char path[128];
    char *extension = ".txt";
    trim(filename);

    strncpy(path, filename, sizeof(filename));
    strncat(path, extension, (sizeof(path) - strlen(path)));
    result = fopen("/home/mr_nachtigal/projects/sniff_last/output/eth0.txt", "w");

    for (int i = 0; i < list.count; i++)
    {
        fprintf(result, RESULT_FORMAT_OUT, list.ips[i].address, list.ips[i].count);
    }

    fclose(result);
}

ListIp ReadFromResult(char *filename)
{
    ListIp list;
    char path[128];
    char *extension = ".txt";

    trim(filename);
    strncpy(path, filename, sizeof(filename));
    strncat(path, extension, (sizeof(path) - strlen(path)));

    result = fopen("/home/mr_nachtigal/projects/sniff_last/output/eth0.txt", "r");
    if(result == NULL)
    {
        result = fopen("/home/mr_nachtigal/projects/sniff_last/output/eth0.txt", "w");
        fclose(result);
        result = fopen("/home/mr_nachtigal/projects/sniff_last/output/eth0.txt", "r");
    }
    int capacity = 1;
    int count = 0;

    Ip *ips = malloc(capacity * sizeof(Ip));

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, result))
    {
        Ip ip;
        sscanf(line, RESULT_FORMAT_IN, ip.address, &ip.count);

        if (count == capacity)
        {
            capacity++;
            ips = realloc(ips, capacity * sizeof(Ip));
        }

        ips[count] = ip;
        count++;
    }

    fclose(result);
    list.ips = ips;
    list.count = count;
    return list;
}

int CheckIfFileExist(char *filename)
{
    char path[128];
    char *extension = ".txt";

    trim(filename);
    strncpy(path, filename, sizeof(filename));
    strncat(path, extension, (sizeof(path) - strlen(path)));

    if (access(path, F_OK) == 0)
    {
        return 1;
    }
    
    return 0;
}

void trim(char *str)
{
    int len = strlen(str);
    while (len > 0 && isspace(str[len - 1]))
    {
        str[--len] = '\0';
    }
}