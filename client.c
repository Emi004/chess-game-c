#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <locale.h>
#include "ui.h"

#define BOARD_T_SIZE 1168
#define BOARD_BYTES 1024
#define BOARD_AND_MOVE_BYTES 1029

int sockfd;

void close_game(){
	endwin();
	close(sockfd);
	printf("You left the game\n");
	exit(0);
}

int main(){
	setlocale(LC_ALL, "");
	int first_turn = 1,invalid_move = 0;
	struct sockaddr_in server; 
	char username[50],buffer[BOARD_T_SIZE + 2];
	char *end_of_username;
	char turn,ch;
	long bytes_read;
	board_t b;
	MOVE_T move;
	struct sigaction sig;

	memset(&sig,0,sizeof(struct sigaction));
	sig.sa_handler = close_game;
	if(sigaction(SIGINT,&sig,NULL) < 0){
		perror("signal");
		exit(1);
	}

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
	turn = buffer[0];
	end_of_username = strchr(buffer,'\n');
	*end_of_username = '\0';
	printf("Congrats you are playing chess against %s\n",buffer+1);
	memcpy(&b,end_of_username+1,BOARD_BYTES);
	if(turn == '0')
		init_ui(b,"white");
	else
		init_ui(b,"black");


	while(1){

		if((turn == '1' || first_turn == 0) && invalid_move == 0){    //read opponent's move
			bytes_read = read(sockfd,buffer,BOARD_AND_MOVE_BYTES);
			if(strcmp(buffer,"Opponent has disconnected\n") == 0)
				break;
			memcpy(&b,buffer+5,BOARD_BYTES);
			if(strcmp(buffer,"4444") == 0){
				render_board(b);
			}
			else{
				move.from_x = buffer[0] - 97;
				move.from_y = 56 - buffer[1];
				move.to_x = buffer[2] - 97;
				move.to_y = 56 - buffer[3];
				move.move_made = 1;
				ui_render_move(move,b,1);
			}
//			printf("first turn ove\n");
		}
		invalid_move = 0;
		move.move_made = 0;
		do{								//citeste mutarea
			ch = getch();
			if(ch == 27)
				close_game();
			move = ui_return_move(b);
		}while(move.move_made == 0);

		buffer[0] = move.from_x + 97;
		buffer[1] = 56 - move.from_y;
		buffer[2] = move.to_x + 97;
		buffer[3] = 56 - move.to_y;
		buffer[4] = '\0';
//		printf("%s\n",buffer);

		write(sockfd,buffer,5);
		bytes_read = read(sockfd,buffer,sizeof(buffer)-1);	
//		buffer[bytes_read] = '\0';
		if(bytes_read == BOARD_BYTES){
			memcpy(&b,buffer,BOARD_BYTES);
			ui_render_move(move,b,1);
//			printf("Opponent's turn\n");
		}
		else if(bytes_read == BOARD_AND_MOVE_BYTES){
			memcpy(&b,buffer+5,BOARD_BYTES);
			render_board(b);
		}
		else if(strcmp(buffer,"Invalid move\n") == 0){
			ui_render_move(move,b,0);
//			printf("%s",buffer);
			invalid_move = 1;
			continue;
		}
		else if(strcmp(buffer,"You win!\n") == 0 || strcmp(buffer,"You lose.\n") == 0 || strcmp(buffer,"Opponent has disconnected\n") == 0 ||  bytes_read == 0){
			break;
		}
		first_turn = 0;
	}

	endwin();
	printf("%s",buffer);
	close(sockfd);
	return 0;
}