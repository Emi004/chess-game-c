#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BACKLOG 10
#define I_MAX 100

typedef struct{
	char username[50];
	struct sockaddr_in client_addr;
	unsigned int client_addr_len;
	int connfd;
}client;

client *client_history[I_MAX] ;


void *run_match(void *args){
	//client *c = (client *)args;
	
	//printf("%d\n",c->connfd);
	//printf("%d\n",(c+1)->connfd);
	
	//write(c->connfd, (c+1)->username, strlen((c+1)->username));
	//write((c+1)->connfd, c->username, strlen(c->username));
	
	//close(c->connfd);
	//close((c+1)->connfd);
	
	int i = *(int *)args;
	
	write(client_history[i]->connfd, client_history[i+1]->username, strlen(client_history[i+1]->username));
	write(client_history[i+1]->connfd, client_history[i]->username, strlen(client_history[i]->username));
	
	return NULL;
}


int main(){
	int sockfd;  
	struct sockaddr_in server_bind; 
	int i=0,is_client_waiting = 0;
	
	long bytes_read;
	pthread_t thread_handle;
	

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
	printf ("Server listening for connection...\n");
	
	while(1){
		client *c = (client *)malloc(sizeof(client));
		if ((c->connfd = accept(sockfd, (struct sockaddr *)&(c->client_addr), &(c->client_addr_len))) < 0){
			perror("accept");
			exit(1);
		}
		bytes_read = read(c->connfd,c->username,sizeof(c->username));
		c->username[bytes_read] = '\0';
		
		if (!is_client_waiting){
			is_client_waiting =1;
			client_history[i] = c;
			i++;
		}
		else{
			is_client_waiting = 0;
			client_history[i] = c;
			int arg = i-1;
			if(pthread_create(&thread_handle,NULL,run_match,(void *)&arg  )!=0){
				perror("pthread_create() error");
				exit(1);
			}
			if(pthread_detach(thread_handle) != 0)
				perror("pthread_detach() error");
			i++;
			
		}
		if(i == I_MAX)
			i = 0;
		//free(c);
	}


	close(sockfd);
	return 0;
}
