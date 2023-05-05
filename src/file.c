#include "models/ip.c"
#include "models/ifaces.c"
#include "models/list-ip.c"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

FILE *config;
FILE *results_path;
FILE *result;
FILE *logs;

#define MAX_LINE_LENGTH 100
#define RESULTS_FOLDER "results"

const char *RESULT_FORMAT_OUT = "%s %d\n";
const char *RESULT_FORMAT_IN = "%s %d";

char *GetResultFolderPath();
char **GetAllResultPaths(int *count);
void trim(char *str);

void Log(char *message)
{
    logs = fopen("log.txt", "a");
    fprintf(logs, "%s\n", message);
    fclose(logs);
}

void WriteToConfig(char *iface)
{
    config = fopen("config.txt", "w");
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
    config = fopen("config.txt", "r");
    if (config == NULL)
    {
        result = fopen("config.txt", "w");
        fclose(result);
        result = fopen("config.txt", "r");
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

    snprintf(path, 128, "%s/%s%s", RESULTS_FOLDER, filename, extension);
    
    result = fopen(path, "w");

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
    char *extension = "txt";
    
    trim(filename);
    snprintf(path, 128, "%s/%s.%s", RESULTS_FOLDER, filename, extension);

    result = fopen(path, "r");
    if (result == NULL)
    {
        result = fopen(path, "w");
        fclose(result);
        result = fopen(path, "r");
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

ListIp GetAllFromResults()
{
    ListIp list;
    int count_files = 0;
    int *count_ptr = &count_files;
    int capacity = 1;
    int count = 0;
    int found = 0;
    Ip *ips = malloc(capacity * sizeof(Ip));

    char **files = GetAllResultPaths(count_ptr);
    
    for (int i = 0; i < *count_ptr; i++)
    {
        char path[128];
        snprintf(path, 128, "%s/%s", RESULTS_FOLDER, files[i]);
        result = fopen(path, "r");

        char line[MAX_LINE_LENGTH];
        while (fgets(line, MAX_LINE_LENGTH, result))
        {
            found = 0;
            Ip ip;
            sscanf(line, RESULT_FORMAT_IN, ip.address, &ip.count);
            if (count == capacity)
            {
                capacity++;
                ips = realloc(ips, capacity * sizeof(Ip));
            }

            for (int i = 0; i < count; i++)
            {
                if (strcmp(ips[i].address, ip.address) == 0)
                {
                    ips[i].count += ip.count;
                    found = 1;
                    break;
                }
            }

            if(found)
            {
                continue;
            }

            ips[count] = ip;
            count++;
        }
        fclose(result);
    }

    list.ips = ips;
    list.count = count;
    return list;
}

char **GetAllResultPaths(int *count)
{
    DIR *dir;
    struct dirent *ent;
    char **files = NULL;
    int i = 0, num_files = 0;

    dir = opendir(RESULTS_FOLDER);
    if (dir == NULL)
    {
        perror("Unable to open dir");
        exit(EXIT_FAILURE);
    }

    while ((ent = readdir(dir)) != NULL)
    {
        if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL)
        {
            num_files++;
        }
    }

    closedir(dir);

    files = (char **)malloc(sizeof(char *) * (num_files + 1));
    if (files == NULL)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    dir = opendir(RESULTS_FOLDER);
    if (dir == NULL)
    {
        perror("Unable to open direc");
        exit(EXIT_FAILURE);
    }

    while ((ent = readdir(dir)) != NULL)
    {
        if(strlen(ent->d_name) < 3)
        {
            continue;
        }
        if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL)
        {
            files[i] = (char *)malloc(sizeof(char) * (strlen(ent->d_name) + 1));
            if (files[i] == NULL)
            {
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }
            strcpy(files[i], ent->d_name);
            i++;
        }
    }

    closedir(dir);
    files[i] = NULL;
    *count = num_files;
    return files;
}

int CheckIfFileExist(char *filename)
{
    char path[128];
    char *extension = ".txt";

    snprintf(path, 128, "%s/%s%s", RESULTS_FOLDER, filename, extension);

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