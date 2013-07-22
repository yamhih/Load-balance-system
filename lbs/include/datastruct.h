/* 
 * This file define the data struct for this engineering. This file contains
 * all data struct. So if there are any changes we can only changes this 
 * file.
 */

/*
 * define the system varibles to keep the values what we need. We can use
 * comand line paramer to open the debugger function using "on". So that we
 * can see the lb's deal abilities.
 */

unsigned int not_responsed = 0;
unsigned int responsed = 0;

/*
 * This is struct of the what we need
*/

typedef struct {
	// the source id
	unsigned src_id;
	// the goal id
	unsigned dst_id;
	// response time request, the value is as same as the request
	unsigned usr_id;
	// 1. time response, 2. heart jump request,3. heart response
	unsigned msg_type;
	// The message	
	char data[32];
}t_msg;

struct server {
	// the distinct id of server
	int server_id;
	// set the server in using
	int flags;
	// the remote server port
	short port;
	// the remote server ip
	char ip[39];
};

struct client {
	// the remote client id
	int client_id;
	// the remote client port
	short port;
	// the remote client ip
	char ip[39];
};

// it is ready for thread_create
struct thread_args {
	// the client fd
	int fd;
	// the server count
	int servc;
	// the server set
	struct server *serv;
	// the client set	
	struct client *clt;
	// the system message struct
	t_msg msg;
};
