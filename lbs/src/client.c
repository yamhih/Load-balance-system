#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get the lb port and ip address
#include "config.h"

// get the t_msg data struct
#include "datastruct.h"

void init_t_msg(t_msg *msg)
{
	msg->src_id = getpid();
	msg->dst_id = LB_ID;
	msg->usr_id = getpid();
	msg->msg_type = 2;
	strcpy(msg->data, "Hello");
}

void client()
{
	int sockfd, length;
	char *host = "localhost";
	struct sockaddr_in address;
	struct hostent *hostinfo;
	t_msg msg;
	char buff[128] = "";
	int size = sizeof(buff);

	hostinfo = gethostbyname(host);
	if (!hostinfo) {
		fprintf(stderr, "Can not get host information\n");
		exit(-1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_addr.s_addr = inet_addr(IP);
	address.sin_family = AF_INET;
	address.sin_port = htons(LB_PORT);
	length = sizeof(address);
	if (connect(sockfd, (struct sockaddr *)&address, length) == -1) {
		fprintf(stderr, "Connect failed, please try later!\n");
		exit(-3);
	}

	// init the t_msg data struct
	init_t_msg(&msg);
	while (1) {
		write(sockfd, &msg, sizeof(msg));
		read(sockfd, buff, size);
		length = strlen(buff);
		printf("Read %d bytes: %s\n", length, buff);
		sleep(2);
	}
	close(sockfd);
}

int main(int argc, char *argv[])
{
	client();

	return 1;
}
