#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// get the server and lb infomation
#include "config.h"

// define the data struct
#include "datastruct.h"

// the function declaration
#include "funcdec.h"

int lb_log(char *msg, int size)
{
	time_t curtime = time(NULL);
	char *ct = ctime(&curtime);
	FILE *fp;

	fp = fopen("../log/lb.log", "a");
	if (fp) {
		fwrite(ct, sizeof(ct), 1, fp);
		fwrite(msg, size, 1, fp);
		fputs("\n\n", fp);
	}
	fclose(fp);
}

void printflb(void)
{
	printf("lb:>>");
}

int init_server_count(void)
{
	int servc;

	servc = SERVC;
	if (servc > SERVER_MAX || servc <= 0)
		return 0;

	return servc;
}

void send_msg_to_server(char *host,struct server serv,char *buff,int size)
{
	int sockfd, length;
//	host = "localhost";
	struct sockaddr_in address;
	struct hostent *hostinfo;
	int i = 4;

/* IF ther is a extern host, you can add this.
 	 hostinfo = gethostbyname(host);
	if (!hostinfo) {
		fprintf(stderr, "Can not get host information\n");
		exit(-1);
	}

*/	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	address.sin_addr.s_addr = inet_addr(serv.ip);
	address.sin_family = AF_INET;
	address.sin_port = htons(serv.port);
	length = sizeof(address);
	if (connect(sockfd, (struct sockaddr *)&address, length) == -1) {
		fprintf(stderr, "Connect failed, please try later!\n");
		lb_log("Connected failed!", 30);
		exit(-3);
	}
	while (i >= 0) {
		write(sockfd, buff, size);
		read(sockfd, buff, size);
		i--;
	}
	close(sockfd);
}


int lb(int servc, struct server *serva)
{
	int cfd, sfd;
	struct sockaddr_in c_addr;
	struct sockaddr_in s_addr;
	int res = 0;
	int length;
	length = sizeof(s_addr);

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		printflb();
		perror("Fater error: ");
		lb_log("Open socket failed!", 30);
		exit(-1);
	}

	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(LB_PORT);
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sfd, (struct sockaddr*)&s_addr, length);
	if (res != 0) {
		close(sfd);
		printflb();
		perror("Fater error: ");
		lb_log("Bin port failed!", 30);
		exit(-2);
	}

	listen(sfd, 5);
	printflb(); printf("LB program begin....");
	while (1) {
		struct thread_args ta;
		pthread_t p_thread;
		cfd = accept(sfd, (struct sockaddr *)&s_addr, &length);
		ta.fd = cfd; ta.serv = serva; ta.servc = servc;
		if (cfd != -1) {
			res = pthread_create(&p_thread, NULL, deal_request,
				(void *)(&ta));
			if (res != 0) {
				printflb();
				perror("Fater error: ");
				lb_log("Create thread failed!", 30);
				close(cfd);
				continue;
			}
			res = pthread_join(p_thread, NULL);
			if (res != 0) {
				printflb();
				perror("Fater error: ");
				lb_log("THread join failed!", 30);
			}
		}
		close(cfd);
	}
	close(sfd);

	return EXIT_SUCCESS;
}

int is_right_dst(t_msg client_msg)
{
	if (client_msg.dst_id == LB_ID)
		return 0;
	return 1;
}

int select_server(struct server *serva, int servc, int *server_id)
{
	int i = 0;
	short flags = 1;

	while ((flags++) >= 0) {
		srand((unsigned)time(NULL));
		int rad = rand() % servc;
		if (serva[rad].flags == 0) {
			*server_id = serva[rad].server_id;
			flags = 0;
			return 1;
		}
	}

	return 0;
}

int send_msg_to_client(struct thread_args ta, char *buff, int length)
{
	send(ta.fd, buff, length, 0);

}

void *deal_request(void *msg)
{
	t_msg client_msg;
	int server_id, res;
	char buff[128];
	struct thread_args *ta = (struct thread_args*)msg;
	int fd = (ta->fd);
	int length = sizeof(client_msg);

	memset(buff, '\0', length);
	if (read(fd, &client_msg, length)) {
		if (is_right_dst(client_msg) == 0) {
			res=select_server(ta->serv, ta->servc, &server_id);
			if (res) {
			    client_msg.src_id = getpid();
			    client_msg.dst_id = server_id;
			    client_msg.usr_id = getpid();
			    client_msg.msg_type = 2;
			    strcpy(client_msg.data, "Time");
			    send_msg_to_server(NULL, ta->serv[server_id],
				buff, sizeof(buff));
			    send_msg_to_client(*ta, buff, sizeof(buff));
			} else
				not_responsed++;
			
		} else
			not_responsed++;
	}
	close(fd);
	pthread_exit(NULL);
}

void init_server(int servc, struct server *serva)
{
	int i = 0;

	for (; i < servc; i++) {
		serva[i].server_id = i+1;
		serva[i].flags = 0;
		serva[i].port = SERVER_PORT;
		strcpy(serva[i].ip, IP);
	}
}

int main(int argc, char *args[])
{
	int servc;

	servc = init_server_count();
	if (servc) {
		// use c99 standrand
		struct server server_set[servc];
		struct client client_set[CLIENT_MAX];
		init_server(servc, server_set);
		lb(servc, server_set);

	} else {
		printflb();
		perror("Fater error: ");
		lb_log("Init server count failed!", 30);
		exit(0);
	}

	return EXIT_SUCCESS;
}
