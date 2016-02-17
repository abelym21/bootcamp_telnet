#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>

#define MESSAGE_LEN 1024

unsigned short getHtonsNum(char *port);


int main(int argc, char *argv[])
{
	char optChar;					//option character
	int sockFd;						//client Socket File descriptor
	char port[4] = {'\0'};			//port
	char ip[15] = {'\0'};			//ip address
	struct sockaddr_in server;		//server Socket Address information.
	char message[MESSAGE_LEN];		//message.
	char response[MESSAGE_LEN];		//response message.
	
	strcpy(port, argv[2]);
	strcpy(ip, argv[1]);
	
	sockFd = socket(AF_INET, SOCK_STREAM, 0); 	//create socket
	if(sockFd ==  -1)
	{
		perror("Error : cannot create socket.\n");
	}
	fprintf(stdout, "Socket created.\n");

	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(getHtonsNum(port));

	if(connect(sockFd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Error : Connect failed.\n");
		return 1;
	}
	fprintf(stdout, "Connect successed.\n");

//	while(1)
//	{
		printf("Enter message :  \n");
		scanf("%s", message);

		if(send(sockFd, message, strlen(message), 0 ) < 0)
		{
			fprintf(stderr, "Error : send failed.\n");
			return 1;
		}
		
		if(recv(sockFd, response, MESSAGE_LEN, 0) < 0 )
		{
			fprintf(stderr, "Error : recv failed.\n");
		//	break;
			return;
		}

		fprintf(stdout, "Server respond : \n");
		fprintf(stdout, "%s\n", response);
//	}
	close(sockFd);
	
	return 0;
}

unsigned short getHtonsNum(char *port)
{
	int i = 0;
	unsigned short result = 0;
	for(i = 0; i < strlen(port); i++)
	{
		result = result*10 + (unsigned short)(port[i]- 48);
		printf("%hd\n", result);
	}
	return result;
}
