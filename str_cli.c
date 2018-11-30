#include <pthread.h>
#include "readline.c"
#define MAXLINE 4096
void *copyto(void *);
static int	sockfd;		/* global for both threads to access */
static FILE	*fp;
static int rec;
static char sender[MAXLINE];
static char username[MAXLINE];
void
str_cli(FILE *fp_arg, int sockfd_arg,char* uname)
{

	pthread_t	tid;
	int n;
	char sendto[MAXLINE];
	char recvline[MAXLINE];



	sockfd = sockfd_arg;	/* copy arguments to externals */
	fp = fp_arg;
	strcpy(username,uname);

	//send setname command
	//checprintf("%s\n",account);
	strcat(uname,"\n");
	write(sockfd, uname, strlen(uname));

	pthread_create(&tid, NULL, copyto, NULL);

	while((n = readline(sockfd, recvline, MAXLINE)) > 0){
		//printf("*%s",recvline);
		if(strncmp(recvline,"file to",7)==0){
			//printf("recvline=%s\n",recvline);
			readline(sockfd, recvline, MAXLINE);
			recvline[strlen(recvline)-1]='\0';
			strcpy(sender,recvline);
			printf("%s want to send a file to you.Do you want it?[y/n]:",sender);
			fflush(stdout);
			rec=1;
			while(rec==1) ;

		}else{
			fputs(recvline, stdout);	
		}
	}
}

void *
copyto(void *arg)
{
	int n;
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	char filepath[MAXLINE];
	char buf[MAXLINE];
	FILE *send_fp,*receive_fp;

	strcpy(filepath,"./");
	strcat(filepath,username);
	strcat(filepath,"/");

	while (fgets(sendline, MAXLINE, fp) != NULL){
		//if user wants to send a file
		if(strncmp(sendline,"file to",7)==0){
			write(sockfd, sendline, strlen(sendline));
			//get file name
			char *colon = strchr(sendline,':');
			int colon_idex = colon-sendline;
			char *enter= strchr(sendline,'\n');
			*(enter)='\0';
			char *filename=&sendline[colon_idex+1];
			printf("filename=%s\n",filename);

			//open file
			strcpy(buf,filename);
			if((send_fp=fopen(buf,"rb"))==NULL){
				printf("Can't find this file\n");
				continue;
			}

			//read file and send to socket
			while(!feof(send_fp)){
				int numBytes = fread(sendline,sizeof(char),sizeof(sendline),send_fp);
				//printf("%s\n",sendline);
				write(sockfd,sendline,numBytes);
				//printf("s=%s",sendline);
			}
			fclose(send_fp);
			strcpy(sendline,"file end\n");
			write(sockfd,sendline,strlen(sendline));
		}else if(rec==1&&strcmp(sendline,"y\n")==0){
			char path[MAXLINE];
			strcpy(path,filepath);
			//get file name
			readline(sockfd, recvline, MAXLINE);
			strcat(path,recvline);
			//printf("filename=%s",recvline);
			printf("path=%s\n",path);
			char *enter= strchr(path,'\n');
			*(enter)='\0';	
			receive_fp=fopen(path,"wb");
			//read file
			while((n = readline(sockfd, recvline, MAXLINE)) > 0){
				if(strcmp(recvline,"file end\n")==0) break;
				//printf("*%s",recvline);
				fwrite(recvline,sizeof(char),strlen(recvline),receive_fp);	
			}
			fclose(receive_fp);
			printf("You have received the file.\n");
			//tell user1 that u accept this file
			strcpy(sendline,"filemsg to ");
			strcat(sendline,sender);
			strcat(sendline,":");
			strcat(sendline,username);
			strcat(sendline," has received your file.\n");
			write(sockfd, sendline, strlen(sendline));
		
			rec=0;
		}else if(rec==1&&strcmp(sendline,"n\n")==0){
			while((n = readline(sockfd, recvline, MAXLINE)) > 0){
				if(strcmp(recvline,"file end\n")==0) break;
			}
			//tell user1 that u dont accept this file
			strcpy(sendline,"filemsg to ");
			strcat(sendline,sender);
			strcat(sendline,":");
			strcat(sendline,username);
			strcat(sendline," rejects your file.\n");
			write(sockfd, sendline, strlen(sendline));

			rec=0;
		}
		else{
			write(sockfd, sendline, strlen(sendline));
		}
	}

	shutdown(sockfd, SHUT_WR);	/* EOF on stdin, send FIN */

	return(NULL);
		/* 4return (i.e., thread terminates) when EOF on stdin */
}
