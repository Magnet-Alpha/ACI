#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

void *send_to_client(int sock)
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


int main()
{
	char buffer[1024];
	
	int sock = socket(AF_INET, SOCK_STREAM,0);
	
	if(sock == -1)
	{
		perror("socket");
		return errno;
	}
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(2000);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr)) < 0)
	{
		perror("bind");
		return errno;
	}
	
	if(listen(sock, 1) == -1)
	{
		perror("listen");
		return errno;
	}

	int sock1;
	struct sockaddr_in sin1;
	socklen_t sinsize1 = sizeof(struct sockaddr_in);

	if((sock1 = accept(sock, (struct sockaddr *)&sin1, &sinsize1)) < 0)
	{
		perror("accept");
		return errno;
	}

	if(read(sock1, &buffer, sizeof(buffer)) == -1)	//qui se co?
        {
                perror("read nom");
                return errno;
        }
	printf("%s est connecté\n",buffer);
	int connecter = 1;

	pthread_t send_to_client_thread;


        if (pthread_create(&send_to_client_thread, NULL, send_to_client, sock1))
        {
                fprintf(stderr, "Error creating thread\n");
                return 1;
        }


	while(connecter)
	{
		read(sock1, buffer, sizeof(buffer));
		if(strncmp(buffer, "END", 10) == 0)
		{
			connecter = 0;
			write(sock1, "END", 4);
			close(sock);
		}
		else
			printf("%s\n", buffer);
	}
	close(sock1);
	pthread_cancel(send_to_client_thread);

	return 0;
}
