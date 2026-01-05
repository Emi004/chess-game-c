#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "ui.h"

#define BOARD_T_SIZE 1168

board_t binary_to_board_t(char buffer[]);


int main(){
	int sockfd,first_iteration=1;
	struct sockaddr_in server; 
	char ch,username[50],buffer[BOARD_T_SIZE + 2];
	long bytes_read;
	board_t b;
	MOVE_T move;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
	{
		perror("socket");
		exit(1);
	}

	memset(&server, 0, sizeof(server)); 
	server.sin_family = AF_INET;		
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(4555); 
	
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
	
	while(1){
		bytes_read = read(sockfd,buffer,sizeof(buffer)-1);	
		buffer[bytes_read] = '\0';
		b = binary_to_board_t(buffer);
		
		if(first_iteration){
			init_ui(b);
			first_iteration = 0; // needs change for black player
		}

		ch = getch();
		move.move_made = 0;

		do{
			move = ui_return_move(ch, b);
		}while(move.move_made == 0);

		write(sockfd,&move,sizeof(move));
		bytes_read = read(sockfd,buffer,sizeof(buffer)-1);	
		buffer[bytes_read] = '\0';

		

		if(strcmp(buffer,"Invalid move, enter another move\n") == 0){
			ui_render_move(move,b,0);
			printf("%s",buffer);
			continue;
		}
		else if (strcmp(buffer,"Opponent's turn\n") == 0){
			ui_render_move(move,b,1);
			printf("%s",buffer);
		}
		else if(strcmp(buffer,"You win!\n") == 0 || strcmp(buffer,"You lose.\n") == 0 || bytes_read == 0){
			break;
		}

	}
	printf("%s",buffer);

	endwin();
	close(sockfd);
	return 0;
}









