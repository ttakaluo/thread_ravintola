#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

//addrinfo-socket.c used as an guideline

int main()
{
	char *hostname = "localhost";
	char *service = "80";
	struct addrinfo hints, *res;
	int err;
	int sock;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	if ((err = getaddrinfo(hostname, service, &hints, &res)) != 0) {
  		printf("error %d : %s\n", err, gai_strerror(err));
  		return 1;
	}

	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (sock < 0) {
  		perror("socket");
  		return 1;
	}

	if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
 		perror("connect");
  		return 1;
	}

	freeaddrinfo(res);

	return 0;
}
