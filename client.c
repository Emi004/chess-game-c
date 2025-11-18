#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(){
	int sockfd;
	struct sockaddr_in server; 
	char username[50],buffer[50];
	long bytes_read;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
	{
		perror("socket");
		exit(1);
	}

	memset(&server, 0, sizeof(server)); 
	server.sin_family = AF_INET;		
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(4556); 
	
	printf("Type your username: ");
	fgets(username,49,stdin);
	
	if ((connect(sockfd, (struct sockaddr *)&server, sizeof(server))) < 0) // realizarea conexiunii cu serverul
	{
		perror("connect");
		exit(1);
	}
	
	write(sockfd, username, strlen(username));
	printf("Connected to the server\n");
	bytes_read = read(sockfd,buffer,sizeof(buffer));
	buffer[bytes_read] = '\0';
	printf("Congrats you are playing chess against %s\n",buffer);
	
	close(sockfd);
	return 0;
}
