#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "chess_logic.h"

#define BACKLOG 10
#define MAX_MATCHES 20
#define BOARD_T_SIZE 1168
#define BOARD_BYTES 1024
#define BOARD_AND_MOVE_BYTES 1029

typedef struct{
	int players;			//used for matchmaking
	int turn;  				//0 white turn, 1 black turn, -1 for white disconected, -2 for black disconected, 2 for white win, 3 for black win 
	pthread_mutex_t mutex;	
	pthread_cond_t cond;	
	board_t board;
	char last_move[5];			
}match_t;

match_t matches[MAX_MATCHES];

void close_game(int i, char color){
	char buffer[30];
	if(color == 'w'){
		if(matches[i].turn == -1){
			pthread_cond_signal(&(matches[i].cond));
		}
		else if(matches[i].turn == -2){
			sprintf(buffer,"Opponent has disconnected\n");
			write(matches[i].board.white.connfd,buffer,strlen(buffer) + 1);
			pthread_mutex_destroy(&(matches[i].mutex));
			pthread_cond_destroy(&(matches[i].cond));
			matches[i].players = 0;
		}
		else if(matches[i].turn == 2){
			pthread_cond_signal(&(matches[i].cond));
			sprintf(buffer,"You win!\n");
			write(matches[i].board.white.connfd,buffer,strlen(buffer) + 1);
		}
		else if(matches[i].turn == 3){
			sprintf(buffer,"You lose.\n");
			write(matches[i].board.white.connfd,buffer,strlen(buffer) + 1);
			pthread_mutex_destroy(&(matches[i].mutex));
			pthread_cond_destroy(&(matches[i].cond));
			matches[i].players = 0;
		}
		close(matches[i].board.white.connfd);
	}
	else if(color == 'b'){
		if(matches[i].turn == -1){
			sprintf(buffer,"Opponent has disconnected\n");
			write(matches[i].board.black.connfd,buffer,strlen(buffer) + 1);
			pthread_mutex_destroy(&(matches[i].mutex));
			pthread_cond_destroy(&(matches[i].cond));
			matches[i].players = 0;
		}
		else if(matches[i].turn == -2){
			pthread_cond_signal(&(matches[i].cond));
		}
		else if(matches[i].turn == 2){
			sprintf(buffer,"You lose.\n");
			write(matches[i].board.black.connfd,buffer,strlen(buffer) + 1);
			pthread_mutex_destroy(&(matches[i].mutex));
			pthread_cond_destroy(&(matches[i].cond));
			matches[i].players = 0;
		}
		else if(matches[i].turn == 3){
			pthread_cond_signal(&(matches[i].cond));
			sprintf(buffer,"You win!\n");
			write(matches[i].board.black.connfd,buffer,strlen(buffer) + 1);
		}
		close(matches[i].board.black.connfd);		
	}

//	printf("Thread inchis cu succes\n");
	pthread_exit(0);
}

void *white_player_thread(void *arg){
	long i = (long)arg;
	long bytes_read;
	char buffer[BOARD_T_SIZE+1] = "0";
	int return_value, first_turn = 1,special_move = 0;

	memcpy(buffer+1,matches[i].board.black.username,strlen(matches[i].board.black.username));
	memcpy(buffer+strlen(matches[i].board.black.username)+1,&(matches[i].board),BOARD_BYTES);
	write(matches[i].board.white.connfd,buffer,strlen(matches[i].board.black.username) + BOARD_BYTES + 1);

	while(1){ 
		pthread_mutex_lock(&(matches[i].mutex));
		while(matches[i].turn == 1)
			if(pthread_cond_wait(&(matches[i].cond),&(matches[i].mutex)) !=0){
				printf("my fucking conditional variable\n");
			}	
		pthread_mutex_unlock(&(matches[i].mutex));
		if(matches[i].turn != 0)
			close_game(i,'w');

		if(!first_turn){ 			//gets move from opponent's turn
			strcpy(buffer,matches[i].last_move);
			memcpy(buffer+5,&(matches[i].board),BOARD_BYTES);
			write(matches[i].board.white.connfd,buffer,BOARD_AND_MOVE_BYTES);
		}

		bytes_read = read(matches[i].board.white.connfd,buffer,sizeof(buffer)-1); 
		if (bytes_read == 0){
			matches[i].turn = -1;
			close_game(i,'w');
		}
		
//		printf("%ld %s\n",bytes_read,buffer);
		while((return_value = chess_main(&(matches[i].board),0,buffer)) == 0){ //return 0 for invalid move, 1 for valid move, 2 for white checkmate, 3 for black chekmate, 4 for special move that needs the entire table to be re-rendered
			sprintf(buffer,"Invalid move\n");
//			printf("why are we here white twin\n");
			fflush(stdout);
			write(matches[i].board.white.connfd,buffer,35);
			bytes_read = read(matches[i].board.white.connfd,buffer,sizeof(buffer)-1);
			if (bytes_read == 0){
				matches[i].turn = -1;
				close_game(i,'w');
			}
//			printf("%d\n",return_value);
//			fflush(stdout);
		}
		strcpy(matches[i].last_move,buffer);
//		printf("%d\n", return_value);
		fflush(stdout);
		if(return_value == 2){
			matches[i].turn = 2;
			close_game(i,'w');
		}
		else if(return_value == 3){
			matches[i].turn = 3;
			close_game(i,'w');
		}
		else if(return_value == 4){
//			printf("castiling\n");
			special_move = 1;
			memset(buffer,'4',4);
			buffer[4] = '\0';
			strcpy(matches[i].last_move,buffer);
			memcpy(buffer+5,&(matches[i].board),BOARD_BYTES);
			write(matches[i].board.white.connfd,buffer,BOARD_AND_MOVE_BYTES);
		}

		first_turn = 0;
		matches[i].turn = 1;
		pthread_cond_signal(&(matches[i].cond));

		if(special_move == 0){
			memcpy(buffer,&(matches[i].board),BOARD_BYTES);
			write(matches[i].board.white.connfd,buffer,BOARD_BYTES);
		}else{
			special_move = 0;
		}
		
//		print_board(&(matches[i].board)); //
	}
	return NULL;
}

void *black_player_thread(void *arg){
	long i = (long)arg;
	long bytes_read;
	char buffer[BOARD_T_SIZE+1] = "1";
	int return_value,special_move = 0;


	memcpy(buffer+1,matches[i].board.white.username,strlen(matches[i].board.white.username));
	memcpy(buffer+strlen(matches[i].board.white.username)+1,&(matches[i].board),BOARD_BYTES);
	write(matches[i].board.black.connfd,buffer,strlen(matches[i].board.white.username) + BOARD_BYTES + 1);

	while(1){ 
		pthread_mutex_lock(&(matches[i].mutex));
		while(matches[i].turn == 0)
			if(pthread_cond_wait(&(matches[i].cond),&(matches[i].mutex)) !=0){
				printf("my fucking conditional variable\n");
			}	
		pthread_mutex_unlock(&(matches[i].mutex));
		
		if(matches[i].turn != 1)
			close_game(i,'b');

		strcpy(buffer,matches[i].last_move);
		memcpy(buffer+5,&(matches[i].board),BOARD_BYTES);
		write(matches[i].board.black.connfd,buffer,BOARD_AND_MOVE_BYTES);

		bytes_read = read(matches[i].board.black.connfd,buffer,sizeof(buffer)-1);
		if(bytes_read == 0){
			matches[i].turn = -2;
			close_game(i,'b');
		}

		while((return_value = chess_main(&(matches[i].board),1,buffer)) == 0){
			sprintf(buffer,"Invalid move\n");
//			printf("why are we here black twin\n");
//			fflush(stdout);
			write(matches[i].board.black.connfd,buffer,35);
			bytes_read = read(matches[i].board.black.connfd,buffer,sizeof(buffer)-1);
			if(bytes_read == 0){
				matches[i].turn = -2;
				close_game(i,'b');
			}
//			printf("%d %s\n",return_value,buffer);
//			fflush(stdout);
		}
//		printf("%d\n",return_value);
		strcpy(matches[i].last_move,buffer);
		if(return_value == 3){
			matches[i].turn = 3;
			close_game(i,'b');
		}
		else if(return_value == 2){
			matches[i].turn = 2;
			close_game(i,'b');
		}
		else if(return_value == 4){
			special_move = 1;
			memset(buffer,'4',4);
			buffer[4] = '\0';
			strcpy(matches[i].last_move,buffer);
			memcpy(buffer+5,&(matches[i].board),BOARD_BYTES);
			write(matches[i].board.black.connfd,buffer,BOARD_AND_MOVE_BYTES);
		}

		matches[i].turn = 0;
		pthread_cond_signal(&(matches[i].cond));
		//sprintf(buffer,"Opponent's turn\n");
		if(special_move == 0){
			memcpy(buffer,&(matches[i].board),BOARD_BYTES);
			write(matches[i].board.black.connfd,buffer,BOARD_BYTES);
		}
		else{
			special_move = 0;
		}
//		print_board(&(matches[i].board));
	}

	return NULL;
} 

void init_player(player_t *p,int color){
	p->can_castle_long = 1;
	p->can_castle_short = 1;
	p->color = color;
	p->is_in_check = 0;
}

void init_match(int i){
//	matches[i].players=0;
	matches[i].turn=0;
//	matches[i].mutex = PTHREAD_MUTEX_INITIALIZER;
	if(pthread_mutex_init(&(matches[i].mutex),NULL) !=0){
		perror("init mutex");
		exit(1);
	}
	if(pthread_cond_init(&(matches[i].cond),NULL) !=0){
		perror("init cond");
		exit(1);
	}
	matches[i].board = init_board(matches[i].board,matches[i].board.white,matches[i].board.black);
	init_player(&(matches[i].board.white),0);
	init_player(&(matches[i].board.black),1);
}

int main(){
	int sockfd;  
	struct sockaddr_in server_bind; 
	int k=0;
	long bytes_read,i=0;
	pthread_t thread_handle;	
	pthread_attr_t attr;
	if(pthread_attr_init(&attr) != 0){
		perror("init attr");
		exit(1);
	}
	if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) !=0){
		perror("detach");
		exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		exit(1);
	}
	memset(&server_bind, 0, sizeof(server_bind)); 
	server_bind.sin_family = AF_INET; 
	server_bind.sin_addr.s_addr = INADDR_ANY; 
	server_bind.sin_port = htons(4555); 
	if (bind(sockfd, (struct sockaddr *)&server_bind, sizeof(server_bind)) < 0) {
		perror("bind");
		exit(1);
	}
	
	if (listen(sockfd, BACKLOG) < 0) {
		perror("listen");
		exit(1); 
	}
	for(int j=0;j<MAX_MATCHES;j++)
		matches[j].players = 0;	

	printf ("Server listening for connection...\n");
	
	while(1){
		while(matches[i].players == 2){
			i++;
			k++;
			if(i == MAX_MATCHES)
				i=0;
			if(k == MAX_MATCHES){
				printf("server is full\n");
				k = 0;
				sleep(5);
				//to do: do more idk
			}
		}
		k = 0;
		if (matches[i].players == 0){
			if ((matches[i].board.white.connfd = accept(sockfd, NULL, NULL)) < 0){
				perror("accept");
				exit(1);
			}
							
			bytes_read = read(matches[i].board.white.connfd,matches[i].board.white.username,49);
			matches[i].board.white.username[bytes_read] = '\0';
			matches[i].players = 1;
		}
		else if(matches[i].players == 1){
			if ((matches[i].board.black.connfd = accept(sockfd, NULL, NULL)) < 0){
				perror("accept");
				exit(1);
			}
			init_match(i);
			bytes_read = read(matches[i].board.black.connfd,matches[i].board.black.username,49);
			matches[i].board.black.username[bytes_read] = '\0';
			matches[i].players = 2;
			if(pthread_create(&thread_handle,&attr,white_player_thread,(void*)i) != 0){
				perror("create thread");
				exit(1);
			}
			if(pthread_create(&thread_handle,&attr,black_player_thread,(void*)i) != 0){
				perror("create thread");
				exit(1);
			}
		}
	}

	pthread_attr_destroy(&attr);
	close(sockfd);
	return 0;
}