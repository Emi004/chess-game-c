#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "chess_logic.h"

#define BACKLOG 10
#define MAX_MATCHES 20
#define BOARD_T_SIZE 1168
//to do on close signal destroy al mutexes and cond variables


typedef struct{
	int players;
	int turn;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	board_t board;
}match_t;

match_t matches[MAX_MATCHES];


void *white_player_thread(void *arg){
	long i = (long)arg;
	long bytes_read;
	char buffer[BOARD_T_SIZE+1];
	int return_value;

	write(matches[i].board.white.connfd,matches[i].board.black.username,strlen(matches[i].board.black.username)); 
	write(matches[i].board.white.connfd,&(matches[i].board),sizeof(matches[i].board));

	while(1){  //replace with not checkmate
		pthread_mutex_lock(&(matches[i].mutex));
		while(matches[i].turn != 0)
			pthread_cond_wait(&(matches[i].cond),&(matches[i].mutex));	
		pthread_mutex_unlock(&(matches[i].mutex));
		
		bytes_read = read(matches[i].board.white.connfd,buffer,sizeof(buffer)-1);
		buffer[bytes_read]='\0';

		while((return_value = chess_main(matches[i].board,0,buffer)) == 0){
			sprintf(buffer,"Invalid move, enter another move\n");
			write(matches[i].board.white.connfd,buffer,sizeof(buffer));
			bytes_read = read(matches[i].board.white.connfd,buffer,sizeof(buffer)-1);
			buffer[bytes_read]='\0';
		}
		if(return_value == 2){
			sprintf(buffer,"You win!\n");
			break;
		}
		else if(return_value == 3){
			sprintf(buffer,"You lose :(\n");
			break;
		}

		matches[i].turn = 1;
		sprintf(buffer,"Opponents turn\n");
		write(matches[i].board.white.connfd,buffer,sizeof(buffer));
	}
	write(matches[i].board.white.connfd,buffer,sizeof(buffer));
	matches[i].players = 0;

	return NULL;
}

void *black_player_thread(void *arg){
	long i = (long)arg;
	long bytes_read;
	char buffer[BOARD_T_SIZE+1];
	int return_value;

	write(matches[i].board.black.connfd,matches[i].board.white.username,strlen(matches[i].board.white.username)); 
	write(matches[i].board.black.connfd,&(matches[i].board),sizeof(matches[i].board));

	while(1){ 
		pthread_mutex_lock(&(matches[i].mutex));
		while(matches[i].turn != 1)
			pthread_cond_wait(&(matches[i].cond),&(matches[i].mutex));
		pthread_mutex_unlock(&(matches[i].mutex));

		bytes_read = read(matches[i].board.black.connfd,buffer,sizeof(buffer)-1);
		buffer[bytes_read]='\0';

		while((return_value = chess_main(matches[i].board,1,buffer)) == 0){
			sprintf(buffer,"Invalid move, enter another move\n");
			write(matches[i].board.black.connfd,buffer,sizeof(buffer));
			bytes_read = read(matches[i].board.black.connfd,buffer,sizeof(buffer)-1);
			buffer[bytes_read]='\0';
		}
		if(return_value == 3){
			sprintf(buffer,"You win!\n");
			break;
		}
		else if(return_value == 2){
			sprintf(buffer,"You lose :(\n");
			break;
		}

		matches[i].turn = 0;
		sprintf(buffer,"Opponents turn\n");
		write(matches[i].board.black.connfd,buffer,sizeof(buffer));

	}
	write(matches[i].board.white.connfd,buffer,sizeof(buffer));

	return NULL;
}

void init_player(player_t *p,int color){
	p->can_castle_long = 1;
	p->can_castle_short = 1;
	p->color = color;
	p->is_in_check = 0;
}

void init_matches(int j){
	matches[j].players=0;
	matches[j].turn=0;
//	matches[j].mutex = PTHREAD_MUTEX_INITIALIZER;
	if(pthread_mutex_init(&(matches[j].mutex),NULL) !=0){
		perror("init mutex");
		exit(1);
	}
	if(pthread_cond_init(&(matches[j].cond),NULL) !=0){
		perror("init cond");
		exit(1);
	}
	matches[j].board = init_board(matches[j].board,matches[j].board.white,matches[j].board.black);
	init_player(&(matches[j].board.white),0);
	init_player(&(matches[j].board.black),1);
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
	server_bind.sin_port = htons(4556); 
	if (bind(sockfd, (struct sockaddr *)&server_bind, sizeof(server_bind)) < 0) {
		perror("bind");
		exit(1);
	}
	
	if (listen(sockfd, BACKLOG) < 0) {
		perror("listen");
		exit(1); 
	}
	for(int j=0;j<MAX_MATCHES;j++)
		init_matches(j);	

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
				sleep(100);
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
		else{
			if ((matches[i].board.black.connfd = accept(sockfd, NULL, NULL)) < 0){
				perror("accept");
				exit(1);
			}
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

		i++;
		if(i == MAX_MATCHES)
			i = 0;
	}

	pthread_attr_destroy(&attr);
	close(sockfd);
	return 0;
}
