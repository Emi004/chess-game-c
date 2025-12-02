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
#define BOARD_T_SIZE 1024
//to do on close signal destroy al mutexes and cond variables

typedef struct{
	char username[50];
	int connfd;
}client;

typedef struct{
	client white_player;
	client black_player;
	int players;
	int turn;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	board_t board;
}match;

match matches[MAX_MATCHES];


void *white_player_thread(void *arg){
	long i = (long)arg;
	long bytes_read;
	char buffer[BOARD_T_SIZE+1];
	
	write(matches[i].white_player.connfd,matches[i].black_player.username,strlen(matches[i].black_player.username)); 
	write(matches[i].white_player.connfd,&(matches[i].board),sizeof(matches[i].board));

	while(1){  //replace with not checkmate
		pthread_mutex_lock(&(matches[i].mutex));
		while(matches[i].turn != 0)
			pthread_cond_wait(&(matches[i].cond),&(matches[i].mutex));	
		pthread_mutex_unlock(&(matches[i].mutex));
		
		bytes_read = read(matches[i].white_player.connfd,buffer,sizeof(buffer)-1);
		buffer[bytes_read]='\0';
//		while(!validate_move(buffer)){
//			sprintf(buffer,"invalid move\n");
//			write(matches[i].white_player.connfd,buffer,sizeof(buffer));
//			bytes_read = read(matches[i].white_player.connfd,buffer,sizeof(buffer)-1);
//			buffer[bytes_read]='\0';
//		}
//		make_move();

		matches[i].turn = 1;
		sprintf(buffer,"Opponents turn\n");
		write(matches[i].white_player.connfd,buffer,sizeof(buffer));
	}
	return NULL;
}

void *black_player_thread(void *arg){
	long i = (long)arg;
	long bytes_read;
	char buffer[BOARD_T_SIZE+1];
	
	write(matches[i].black_player.connfd,matches[i].white_player.username,strlen(matches[i].white_player.username)); 
	write(matches[i].black_player.connfd,&(matches[i].board),sizeof(matches[i].board));

	while(1){ //replace with not chekmate
		pthread_mutex_lock(&(matches[i].mutex));
		while(matches[i].turn != 1)
			pthread_cond_wait(&(matches[i].cond),&(matches[i].mutex));
		pthread_mutex_unlock(&(matches[i].mutex));

		bytes_read = read(matches[i].black_player.connfd,buffer,sizeof(buffer)-1);
		buffer[bytes_read]='\0';
//		while(!validate_move(buffer)){
//			sprintf(buffer,"invalid move\n");
//			write(matches[i].black_player.connfd,buffer,sizeof(buffer));
//			bytes_read = read(matches[i].black_player.connfd,buffer,sizeof(buffer)-1);
//			buffer[bytes_read]='\0';
//		}
//		make_move();

		matches[i].turn = 0;
		sprintf(buffer,"Opponents turn\n");
		write(matches[i].black_player.connfd,buffer,sizeof(buffer));

	}
	return NULL;
}



//void cleanup(){}

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
//	init_board(matches[j].board);
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
				sleep(100);
				//to do: do more idk
			}
		}
		if (matches[i].players == 0){
			if ((matches[i].white_player.connfd = accept(sockfd, NULL, NULL)) < 0){
				perror("accept");
				exit(1);
			}
			bytes_read = read(matches[i].white_player.connfd,matches[i].white_player.username,49);
			matches[i].white_player.username[bytes_read] = '\0';
			matches[i].players = 1;
		}
		else{
			if ((matches[i].black_player.connfd = accept(sockfd, NULL, NULL)) < 0){
				perror("accept");
				exit(1);
			}
			bytes_read = read(matches[i].black_player.connfd,matches[i].black_player.username,49);
			matches[i].black_player.username[bytes_read] = '\0';
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
