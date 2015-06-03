/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>					//atoi, exit
#include <string.h> 					//memset
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>  					//fork
#include <pthread.h>
#include "print_error.h" 				//error-handling function => void error(char *msg)
#include "talk_to_client.h"  				//child-process read-socket
#include "fifod.h"  //contains hardcoded path	//create a fifo-pipe daemon

#define LOGFILE "/tmp/logfile"

int main(int argc, char *argv[]){

	if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
	}
	int logfile;
	logfile = open(LOGFILE, O_WRONLY | O_APPEND | O_CREAT, 0777); //create log-file

	fifod(logfile);					//create fifo-daemon

	int sockfd;						//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // ipv4 connection, sequenced two-way
	if (sockfd < 0) 
		error("ERROR opening socket");

	struct sockaddr_in serv_addr;			 
	int portno;

	memset((char *) &serv_addr ,0,sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;   //accept all interfaces
	serv_addr.sin_port = htons(portno);		//convert network byte order to host byte order

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	struct sockaddr_in cli_addr;
	socklen_t clilen;

	struct arg_struct {
		int newsockfd;
		int logfile;
		pthread_key_t thr_id_key;
		int * thread_id;
	};

	struct arg_struct args;

	pthread_t thread;
	int * thread_id;
	pthread_key_t thr_id_key;

	thread_id = (int * ) malloc(sizeof(int));
	pthread_key_create(&thr_id_key, NULL);

	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	int i = 0;					//start creating threads

	while(1){						//Loop to listen and spawn new threads
	
     		listen(sockfd,5);
		clilen = sizeof(cli_addr);
		int newsockfd;
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");

		args.newsockfd = newsockfd;
		args.logfile = logfile;
		args.thread_id = thread_id;
		
		*thread_id = i;

		printf("Creating thread\n");

		if(pthread_create (&thread, &thread_attr, talk_to_client, &args) )
		{
			printf("Error perror\n");
			error("Error creating a thread.");
 		}
		i++;
	} 
	printf("Exited while(1) ?!?!? \n");
	pthread_exit(NULL);
	return 0; 					//should never get here
}
