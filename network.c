#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "utils_v2.h"
#include "network.h"

#define MAX_ZOMBIES 3

static int indexTab = 0;

const int tabPorts[NBR_PORTS] = {
	SERVER_PORT1,
	SERVER_PORT2,
	SERVER_PORT3,
	SERVER_PORT4,
	SERVER_PORT5,
	SERVER_PORT6,
	SERVER_PORT7,
	SERVER_PORT8,
	SERVER_PORT9,
	SERVER_PORT10};

int testAndConnectPorts(const char *ip, int *sockFdPortsConnectedTab)
{
	for (size_t i = 0; i < NBR_PORTS; i++)
	{
		int sockfd = ssocket();
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(tabPorts[i]);
		inet_aton(ip, &addr.sin_addr);
		int res = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
		if (res == 0)
		{
			printf("Connection établie sur le port : %d\n", tabPorts[i]);
			sockFdPortsConnectedTab[indexTab] = sockfd;
			indexTab++;
		}
		else
		{
			sclose(sockfd);
		}
	}
	return indexTab;
}

int initSocketServer(int port)
{
	int sockfd = ssocket();

	int option = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

	sbind(port, sockfd);

	slisten(sockfd, MAX_ZOMBIES);

	return sockfd;
}

void disconnectZombies(int nbrToDisconnect, int *tabToDisconnect)
{
	for (size_t i = 0; i < nbrToDisconnect; i++)
	{
		sclose(tabToDisconnect[i]);
	}
}