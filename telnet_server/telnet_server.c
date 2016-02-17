#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#define MESSAGE_LEN 9999
#define PAYLOAD_LEN 999
#define CLIENT_MAX 100

void *respond(void *pclientSockFd);
void printPath(char *path);
void printError(char *error);
void setServer(char *port, int*serverSocknum);
void initClientArray(int *clientSockets);
char *path;							//current directory path


int main(int argc, char argv[])
{
	int serverSocknum = 0; 				//server socket number
	//int clientSockFd;				//client socket File Descriptor
	int connectNum = 0;					//connection number
	char port[4];					//port number
	struct sockaddr_in clientaddr;	//client Socket structure
	int newClientSocket;			//new Clienst
	int clientFd[CLIENT_MAX];		//clientScokets Array
	socklen_t addrlen;
	pthread_t p_thread;
	//printf("%s\n", argv[1]);
	strcpy(port, "8887");
	path = getenv("PWD");
	(char *)malloc(sizeof(char)*strlen(path));	
	printPath(path);	
	setServer(port, &serverSocknum);
	printf("%d\n", serverSocknum);
	initClientArray(clientFd);				//clientSockets Array Init.
	while(1)
	{
		addrlen = sizeof(clientaddr);
		clientFd[connectNum] = accept(serverSocknum, (struct sockaddr *)&clientaddr, &addrlen);	//accept	
		printf("%d\n", clientFd[connectNum]);
		if(clientFd[connectNum] == -1)
		{
			printf("Error : accept function error.\n");
		}
		else
		{
			printf("connection accepted.\n");

			if( pthread_create(&p_thread, NULL, respond, &clientFd[connectNum]) < 0 )
			{
				perror("Error : cannot create thread.\n");
				exit(0);
			}
			else
			{
				pthread_detach(p_thread);		//disassociate from parent
			}
			//respond(clinetSockFd, path);
		}
		while(clientFd[connectNum] != -1)
		{
			connectNum = ( connectNum + 1) % CLIENT_MAX;
		}
	}
	printf("server exit.\n");

	return 0;
}

void initClientArray(int * clientSockets)
{
	int i = 0;
	for(i = 0; i < CLIENT_MAX; i++)
	{
		clientSockets[i] = -1;	
	}
}

void *respond(void *clientSockFd)
{
	int *pclientSockFd = (int *)clientSockFd;
	char message[MESSAGE_LEN];	//message buffer
	int rcv;					//receive bytes
	int fd;						//file descriptor
	int toRead;					//bytes to read
	
	memset( (void *)message, (int)'\0', MESSAGE_LEN);		//init message buffer
	rcv = recv(*pclientSockFd, message, MESSAGE_LEN, 0);		//receive message
	if(rcv == -1)
	{
		fprintf(stderr, "Error : recv function Error.\n");
	}
	else if(rcv == 0)
	{
		fprintf(stderr, "Error : receive 0 byte.");
	}
	else
	{
		printf("RECEIVE MESSAGE :\n");
		//write(1, message, strlen(message));				//print receive message
		fprintf(stdout,"%s\n", message);
		write(*pclientSockFd, message, strlen(message));	//send
	}
	shutdown(*pclientSockFd, SHUT_RDWR);
	close(*pclientSockFd);
	//free(pclientSockFd);
	*pclientSockFd = -1;
}

void setServer(char *port, int* serverSocknum)
{
	int sockOpt = 1;
	struct addrinfo hints, *res, *ips;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if(getaddrinfo(NULL, port, &hints, &res))
	{
		perror("Error : getaddrinfo function error.\n");
		return;
	}
	for(ips = res; ips != NULL; ips = ips->ai_next)
	{
		*serverSocknum = socket(ips->ai_family, ips->ai_socktype, 0);
		setsockopt(*serverSocknum, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt));
		if(*serverSocknum == -1){
			continue;
		}
		if(bind(*serverSocknum, ips->ai_addr, ips->ai_addrlen) == 0 )
		{
			break;
		}
	}
	freeaddrinfo(res);
	if(listen(*serverSocknum, 10000000) != 0)
	{
		perror("Error : listen function error.\n");
		return;
	}
	
	return; 
}

void printPath(char *path)
{
	printf("--------[CURRENT DIRECTORY PATH]-------------\n");
	printf("%s\n", path);
	printf("---------------------------------------------\n");
}

