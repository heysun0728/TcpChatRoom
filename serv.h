#ifndef __INCLUDE_GUARD_HERE__
#define __INCLUDE_GUARD_HERE__
#define clientLen 100
struct client{
	char name[100];
	char* password;
	int connfd;
};
#endif
extern struct client clients[clientLen];
