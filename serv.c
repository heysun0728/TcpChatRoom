#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/tcp.h>
#include "tcp_listen.c"
#include "str_echo.c"
#include "serv.h"
#define clientLen 100
struct client clients[clientLen];
static void *doit(void *);		// each thread executes this function
int main(int argc, char **argv)
{
	int		listenfd, iptr, *num;
	pthread_t	tid;
	socklen_t	addrlen, len;
	struct sockaddr	*cliaddr;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else{
		printf("usage: tcpserv01 [ <host> ] <service or port>\n");
		exit(1);
	}

	cliaddr = malloc(addrlen);
	for(int i=0;i<clientLen;i++){
		clients[i].connfd=-1;
	}

	while (1) {
		len = addrlen;
		iptr = accept(listenfd, cliaddr, &len);
		num = malloc(sizeof(int));
		//choose clients index		
		(*num)=0;

		
		while((*num)<clientLen){
			if(clients[(*num)].connfd==-1){
			        clients[(*num)].connfd=iptr;	
				break;
			}
			(*num)++;
		}
		if(clients[(*num)].connfd==-1){
			printf("clients is not enough now\n");
			exit(2);
		}
		pthread_create(&tid, NULL, &doit, num);
	}
}

static void * doit(void * num)
{
	int n=*((int*)num);
	struct client* c= &clients[n];

	pthread_detach(pthread_self());
	str_echo(c);		// same function as before 

	//exit
	close(c->connfd);	// done with connected socket 
	clients[n].connfd=-1;

	return(NULL);
}


