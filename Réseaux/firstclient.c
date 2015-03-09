#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
//#include <poll.h>
#include <errno.h>

#define bool int
#define false 0
#define true 1

void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

int lire(char *str, int len)
{
    char *pos = NULL;

    if (fgets(str, len, stdin) != NULL)
    {
        pos = strchr(str, '\n');
        if (pos != NULL)
        {
            *pos = '\0';
        }
        else
        {
            viderBuffer();
        }
        return 1;
    }

    viderBuffer();
    return 0;

}
int main(int argc,char *argv[])
{
        if(argc < 2)
        {
                printf("Usage : %s [address]\n", argv[0]);
                return EXIT_FAILURE;

        }
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1)
        {
                perror("socket");
                return errno;
        }

        struct sockaddr_in sin;
        sin.sin_family = AF_INET; //famille
        sin.sin_port = htons(2000); //le port
        sin.sin_addr.s_addr = inet_addr(argv[1]); //ip

        if(connect(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1)
        {
                perror("connect");
                return errno;
        }

        char buffer[1024];

        bool continuer = true;

        do
        {/*
                lire(buffer, 1024);

                if(write(sock, buffer, sizeof(buffer)) == -1)
                {
                        perror("write");
                        return errno;
                }

                if(strcmp(buffer, "007") == 0) /
                        continuer = false;
                else
                        puts("Message envoye");*/
		if(read(sock, buffer, sizeof(buffer)) == -1)
		{
			perror("read");
			return errno;
		}
		if(strcmp(buffer, "end") == 0)
			continuer = false;
		else
			printf("%s\n", buffer);
                lire(buffer, 1024);

                if(write(sock, buffer, sizeof(buffer)) == -1)
                {
                        perror("write");
                        return errno;
                }

                if(strcmp(buffer, "end") == 0)
                        continuer = false;

        }while(continuer == true);

        close(sock);

        return 0;
}

