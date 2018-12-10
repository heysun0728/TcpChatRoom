#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "serv.h"
#include "readline.c"
#define MAXLINE 4096

void str_echo(struct client* c)
{
	ssize_t n;
	char buf[MAXLINE];
	char user[MAXLINE];
	char input_format[MAXLINE];
	char member[MAXLINE];
	char sendto[MAXLINE];
	int sockfd=c->connfd;
	FILE *fp;
	int isFind;
again:

	while ( (n = read(sockfd, buf, MAXLINE)) > 0){
		printf("*%s",buf);
		isFind=0;
		strcpy(member,"\0");
		if(strncmp(buf,"check member",12)==0){
			for(int i=0;i<clientLen;i++){
				if(clients[i].connfd!=-1){
					strcat(member,clients[i].name);
					strcat(member," ");
				}
			}
			strcat(member,"\n");
			write(c->connfd, member, strlen(member));
		}
		else if(strncmp(buf,"to",2)==0){

			//get sendto user name
			char *enter= strchr(buf,'\n');
			*(enter+1)='\0';
			char *colon = strchr(buf,':');
			int colon_idex = colon-buf;
			memcpy(sendto,&buf[3],colon_idex-3);
			char *content=&buf[colon_idex+1];

			//find this user exist or not
			for(int i=0;i<clientLen;i++){
				if(clients[i].connfd!=-1&&strcmp(clients[i].name,sendto)==0){
					write(clients[i].connfd,user,strlen(user)); //print user format
					write(clients[i].connfd,content, strlen(content));
					isFind=1;
				}
			}
			if(!isFind){
				strcpy(buf,"This receiver doesn't exist.\n");
			}else{
				strcpy(buf,"Your message has been delivered.\n");
			}
			write(c->connfd,buf,strlen(buf));
		}
		else if(strncmp(buf,"file to",7)==0){
			//get sendto user name
			char *enter= strchr(buf,'\n');
			*(enter+1)='\0';
			char *colon = strchr(buf,':');
			int colon_idex = colon-buf;
			memcpy(sendto,&buf[8],colon_idex-8);
			char *filename=&buf[colon_idex+1];
			int isFind=0;

			for(int i=0;i<clientLen;i++){
				if(clients[i].connfd!=-1&&strcmp(clients[i].name,sendto)==0){

					//send [file to command]
					write(clients[i].connfd,buf, strlen(buf));
					//printf("*%s\n",buf);

					//give reciever [sender name]
					strcpy(buf,c->name);
					strcat(buf,"\n");
					write(clients[i].connfd,buf,strlen(buf));
					//printf("*%s\n",buf);

					//give reciever file name
					strcpy(buf,filename);
					write(clients[i].connfd,buf,strlen(buf));
					//printf("*%s\n",buf);

					//send data
					while((n = readline(sockfd, buf, MAXLINE)) > 0){
						write(clients[i].connfd,buf,strlen(buf));
						//printf("%s\n",buf);
						if(strcmp(buf,"file end\n")==0) break;
					}
					isFind=1;
				}
			}

			if(!isFind){
				strcpy(buf,"This receiver doesn't exist.\n");
				write(c->connfd,buf,strlen(buf));
			}
			
		}else if(strncmp(buf,"filemsg to",10)==0){
			//get sendto user name
			char *colon = strchr(buf,':');
			int colon_idex = colon-buf;
			memcpy(sendto,&buf[11],colon_idex-11);
			//printf("%s\n",sendto);
			char *content=&buf[colon_idex+1];
			for(int i=0;i<clientLen;i++){
				if(clients[i].connfd!=-1&&strcmp(clients[i].name,sendto)==0){
					write(clients[i].connfd,content,strlen(content));
				}
			}
		
		}else if(strncmp(buf,"find:",5)==0){
			//printf("%s\n",buf);
			FILE* userdata;
			char *enter= strchr(buf,'\n');
			*(enter)='\0';
			char *colon = strchr(buf,':');
			isFind=0;
			char *slash = strchr(buf,'/');
			userdata=fopen("userdata.txt","w");
			userdata=fopen("userdata.txt","r");
			while(fscanf(userdata,"%s",user)!=EOF){
				//printf("%s %s\n",user,colon+1);
				if(strcmp(user,(colon+1))==0){
					//printf("found");
					strcpy(buf,"found");
					write(c->connfd,buf,strlen(buf));
					isFind=1;
				}else if(strncmp(user,(colon+1),slash-colon)==0){
					strcpy(buf,"account repeat");
					write(c->connfd,buf,strlen(buf));
					isFind=2;
				}
			}
			strcpy(buf,"not found");
			if(!isFind){
				//printf("not found");
				//printf("%s\n",buf);
				write(c->connfd,buf,strlen(buf));
			}else if(isFind==1){
				//set user name
				read(sockfd, buf, MAXLINE);
				char *enter= strchr(buf,'\n');
				*(enter)='\0';			
				strcpy(c->name,buf);

				//set user format:
				strcpy(user,"[");
				strcat(user,c->name);
				strcat(user,"]:");

				//write input format:
				strcpy(input_format,"\
\n-------------歡迎來到聊天室--------------\n\
輸入'check member'          =>顯示所有在聊天室的成員\n\
輸入'to user1:hi'           =>只送出hi給user1\n\
輸入'file to user1:file.txt =>傳送file.txt給user1'\n\
-----------------------------------------\n\n");
				write(c->connfd,input_format,strlen(input_format));

			}
			fclose(userdata);
			//printf("****\n");
		}else if(strncmp(buf,"register:",9)==0){
			char *colon = strchr(buf,':');
			FILE* userdata;
			userdata=fopen("userdata.txt","a");
			fwrite(colon+1,sizeof(char),strlen(colon+1),userdata);	
			fclose(userdata);

		}else{	
			if(strncmp(buf,"\n",1)==0) continue;
			char *enter = strchr(buf,'\n');
			*(enter+1)='\0';
			for(int i=0;i<clientLen;i++){
				if(clients[i].connfd!=-1){
					write(clients[i].connfd, user,strlen(user)); //print user format
					write(clients[i].connfd, buf, strlen(buf));  //print what he said
					//printf("%s\n",buf);		
				}
			}
		}
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		exit(2);//err_sys("str_echo: read error");
}


