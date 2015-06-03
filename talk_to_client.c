#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "print_error.h"
#include "fifod.h"
#include "talk_to_client.h"

void * talk_to_client(void * arguments){

	//this is a thread handling incoming communication
	char buffer[256];
	int n;
	
	struct arg_struct *args;
	args = (struct arg_struct *) arguments;
	int thread_id = *(args -> thread_id);

	memset(buffer,0,256);
	pthread_setspecific(args -> thr_id_key, args -> thread_id);

      do{
      	n = read(args -> newsockfd,buffer,255);
      	if (n < 0)
      		error("ERROR reading from socket");
      }
      while (n != 0) ;
      close(args -> newsockfd);
      write(args -> logfile, buffer, strlen(buffer));

	printf("Client thread [%d] is done, exiting.\n", thread_id );
	return 0;
}
