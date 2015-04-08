#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>


#define MAX_SIZE_PACKET   2048
#define MAX_SIZE_BODY     2039
#define MAX_SIZE_CHATROOM 42

void *send_to_server(int sock)
{
	int continu = 1;
	char str1[1024];
	while(continu)
	{
		scanf("%s",str1);
		write(sock, str1, sizeof(str1));
		if(strcmp(str1 ,"END") == 0)
			continu =0;
	}
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	
	if(argc<2)
		return -1;

	char buffer[1024];

	struct sockaddr_in sin;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(2000);
	sin.sin_addr.s_addr = inet_addr(argv[1]);

	if(connect(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1)
        {
                perror("connect");
                return errno;
        }

	write(sock, "baille_v",9);

	    pthread_t send_to_server_thread;

	if (pthread_create(&send_to_server_thread, NULL, send_to_server, sock))
	{
        	fprintf(stderr, "Error creating thread\n");
	        return 1;
    	}
	while(strcmp(buffer, "END") != 0)
	{
		read(sock, buffer,sizeof(buffer));
		printf("%s\n",buffer);
	}
	write(sock, "END", 4);

	pthread_cancel(send_to_server_thread);
	return 0;
}
