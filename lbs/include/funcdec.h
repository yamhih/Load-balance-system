/*
* This file contains the functions declare. We will give all
* details about the functions. We should make you understand what
* the funstions should do.
*/


// we define another printf in my lb
void printflb(void);

/* we shoulkd init the server count to control the deal
* abilities, the server count is beteween 1 an 128
*/
int init_server_count(void);

/* we also should init the server. in fact, we init the
* default port and ip address, you can change it to your
* network environment.
*/
void init_server(int servc, struct server *serva);

/* The main function of this system. It created a virtual
* server to recieve the messages from clients, then it
* based the based the message to create a thread to deal the
* request. In other words, one request will create one thread.
*/ 
int lb(int servc, struct server *serva);

// it defines to judge the destination is it right
int is_right_dst(t_msg client_msg);

/* This is function which call by pthread_create. It deal the request
* from clients and send the request to the remote server, then resend
* the message to the client. After sended the message to clients it exit.
*/
void *deal_request(void *msg);

// select a free server to deal the request
int select_server(struct server *serva, int servc, int *server_id);

// send the message from server to the clients
int send_msg_to_client(struct thread_args ta, char *buff, int length);

// send the message from clients to the server, then return the response
void send_msg_to_server(char *host, struct server serv, \
	char *buff, int size);

// the all operationg will write down to check
int lb_log(char *msg, int size);
