#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// get the server port
#include "config.h"

unsigned int get_request_count;
unsigned int deal_request_count;
unsigned int failed_responsed_count;

void daytime_service(char *buff, int size);
void udpserver(int argc, char *argv[]);

int main(int argc, char *argv[])
{

	udpserver(argc, argv);

	return 0;
}

void daytime_service(char *buff, int size)
{
	char *host = "localhost";
	int sockfd;
	int len, result;
	struct sockaddr_in address;
	struct hostent *hostinfo;
	struct servent *servinfo;

	hostinfo = gethostbyname(host);
	if (!hostinfo) {
		perror("Host: ");
		exit(-1);
	}

	servinfo = getservbyname("daytime", "udp");
	if (!servinfo) {
		perror("Service: ");
		exit(-2);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	address.sin_family = AF_INET;
	address.sin_port = servinfo->s_port;
	address.sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;
	len = sizeof(address);
	result = sendto(sockfd, buff, 1, 0,(struct sockaddr *)
			&address, len);
	result = recvfrom(sockfd, buff, size, 0,
			(struct sockaddr *)&address, &len);
	buff[result] = '\0';
	close(sockfd);
}

void udpserver(int argc, char *argv[])
{
	char *host;
	int sockfd;
	pid_t pid;
	int len, result;
	struct sockaddr_in address;
	char *debugger = "off";

	if (argc == 1)
		host = "localhost";
	else if (argc >= 2 && argc <=3) {
		if (strcpy(argv[1], "on") == 0 ||
			strcpy(argv[2], "on") == 0)
			;

		host = argv[1];
	}

	pid = getpid();
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	address.sin_family = AF_INET;
	address.sin_port = htons(SERVER_PORT);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	len = sizeof(address);
	result = bind(sockfd, (struct sockaddr *)&address, len);
	if (result == -1) {
		perror("Bind: ");
		exit(-2);
	}

	while (1) {
		int client_fd;
		char string[128];
		char buff[128];
		struct sockaddr_in client_address;
		memset(buff, '\0', sizeof(buff));

		// get the time from daytime server
		daytime_service(buff, sizeof(buff));
		result = recvfrom(sockfd, string, sizeof(string), 0,
				(struct sockaddr *)&address, &len);
		sendto(sockfd, buff, sizeof(buff), 0,(struct sockaddr *)
			&address, len);
		close(client_fd);
		sleep(1);
	}
	close(sockfd);
}
