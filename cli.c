#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include "tcp_connect.c"
#include "str_cli.c"
#define MAXLINE 4096
char buf[MAXLINE];
int isMember(int,char*,char*);
int main(int argc, char **argv)
{
	int sockfd,choose;
	char user_command[MAXLINE];
	char account[MAXLINE];
	char password[MAXLINE];

	FILE * userdata;
	pthread_t tid;
	if (argc != 3){
		printf("usage: tcpcli <hostname> <service>\n");
		exit(2);
	}
	
	sockfd = tcp_connect(argv[1], argv[2]);

	while(1){
		printf("請選擇登入或註冊(1/2):");
		scanf("%d",&choose);
		if(choose==1){
			printf("\n-----------登入畫面----------\n\n");
			printf("帳號:");
			scanf("%s",account);
			printf("密碼:");
			scanf("%s",password);
			printf("\n-----------------------------\n");
			if(isMember(sockfd,account,password)==1){
				printf("登入成功\n");
				break;
			}else{
				printf("帳號或密碼輸入錯誤\n");
			}
		}else if(choose==2){
			while(1){
				printf("\n-----------註冊表單----------\n\n");
				printf("帳號:");
				scanf("%s",account);
				printf("密碼:");
				scanf("%s",password);
				printf("\n-----------------------------\n");
				if(isMember(sockfd,account,password)==2){
					printf("此帳號名已被註冊過,請重新註冊\n\n");
				}else{
					//printf("else\n");					
					strcpy(buf,"register:");
					strcat(buf,account);
					strcat(buf,"/");
					strcat(buf,password);
					strcat(buf,"\n");
					write(sockfd,buf,strlen(buf));
					//mkdir for receive file
					if(mkdir(account, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1)
						printf("mkdir fail\nPlease retry.\n");
					printf("註冊成功\n");
					break;
				}
			}

		}else{
			printf("輸入錯誤\n");
		}
	}

	str_cli(stdin,sockfd,account);		/* do it all */
	exit(0);
}

int isMember(int sockfd,char* acc,char* pwd){
	strcpy(buf,"find:");
	strcat(buf,acc);
	strcat(buf,"/");
	strcat(buf,pwd);
	strcat(buf,"\n");
	//printf("buf=%s\n",buf);
	write(sockfd,buf,strlen(buf));
	//receive server respond
	read(sockfd, buf, MAXLINE);
	//printf("buf=%s\n",buf);
	if(strncmp(buf,"found",5)==0){
		//printf("found\n");
		return 1;
	}else if(strncmp(buf,"not found",9)==0){
		//printf("not found\n");
		return 0;
	}else{
		//printf("exist\n");
		return 2;
	}
}




