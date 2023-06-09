#include <net/if.h>
#include "file.c"
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>	
#include <stdlib.h> 
#include <string.h> 

#include <netinet/ip_icmp.h>  
#include <netinet/udp.h>	  
#include <netinet/tcp.h>	  
#include <netinet/ip.h>		  
#include <netinet/if_ether.h> 
#include <net/ethernet.h>	  
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>


#define IFACE_LENGTH 10

int IsRunning = 0;

int IfExistGetId(ListIp list, char *ip);
ListIp AddNewIp(ListIp list, struct in_addr *ip);

Ifaces GetAvailableIfaces()
{
	Ifaces iface;
	struct if_nameindex *if_nidxs, *intf;
	int count = 0;
	int iterator = 0;
	if_nidxs = if_nameindex();
	if (if_nidxs != NULL)
	{
		for (intf = if_nidxs; intf->if_index != 0 || intf->if_name != NULL; intf++)
		{
			count++;
		}

		char **ifaces = malloc(count * sizeof(char *));
		for (intf = if_nidxs; intf->if_index != 0 || intf->if_name != NULL; intf++)
		{
			ifaces[iterator] = malloc(IFACE_LENGTH * sizeof(char));
			strcpy(ifaces[iterator], intf->if_name);
			iterator++;
		}
		if_freenameindex(if_nidxs);
		iface.count = count;
		iface.ifaces = ifaces;

		return iface;
	}
}

void *Start(void *data)
{
	IsRunning = 1;
	char *iface = (char *)data;
	char iface_str[10];
	int saddr_size, data_size;
	struct sockaddr saddr;
	Trim(iface);
	for (int i = 0; i < strlen(iface); i++) 
	{
		iface_str[i] = iface[i];
	}

	unsigned char *buffer = (unsigned char *)malloc(65536);
	int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	int sock_option = setsockopt(sock_raw, SOL_SOCKET, SO_BINDTODEVICE, iface_str, strlen(iface_str) + 1);
	if (sock_raw < 0 || sock_option < 0)
	{
		perror("Socket Error");
		pthread_exit(NULL);
	}

	while (1)
	{
		if (!IsRunning)
		{
			close(sock_raw);
			pthread_exit(NULL);
		}

		saddr_size = sizeof saddr;
		data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_size);
		if (data_size < 0)
		{
			printf("Recvfrom error , failed to get packets\n");
			pthread_exit(NULL);
		}

		struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
		struct in_addr ip_addr;
		ip_addr.s_addr = iph->saddr;

		ListIp list = ReadFromResult(iface);

		int id = IfExistGetId(list, inet_ntoa(ip_addr));
		if (id == -1)
		{
			list = AddNewIp(list, &ip_addr);
		}
		else
		{
			list.ips[id].count++;
		}

		WriteToResult(iface, list);
	}
}

void StopSniffing()
{
	IsRunning = 0;
}

int IfExistGetId(ListIp list, char *ip)
{
	int id = -1;

	for (int i = 0; i < list.count; i++)
	{
		if (strcmp(list.ips[i].address, ip) == 0)
		{
			id = i;
			break;
		}
	}
	return id;
}

ListIp AddNewIp(ListIp list, struct in_addr *ip)
{
	Ip newIp = {1};

	strcpy(newIp.address, inet_ntoa(*ip));

	Ip *newIps = (Ip *)calloc(list.count + 1, sizeof(struct Ip));
	for (int i = 0; i < list.count; i++)
	{
		newIps[i] = list.ips[i];
	}

	newIps[list.count] = newIp;

	list.ips = newIps;
	list.count++;
	return list;
}