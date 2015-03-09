#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <poll.h>
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

int main()
{
        int sock = socket(AF_INET, SOCK_STREAM, 0);

        if(sock == -1)
        {
                perror("socket");
                return errno;
        }

        struct sockaddr_in sin;

        sin.sin_family = AF_INET;
        sin.sin_port = htons(2000);
        sin.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY = localhost
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
        int csock; //socket client
        struct sockaddr_in csin; //struct client
        int sinsize = sizeof(csin);
        if((csock = accept(sock, (struct sockaddr *)&csin, &sinsize)) < 0)
        {
                perror("accept");
                return errno;
        }
        char buffer[1024];
        bool continuer = true;
	printf("on entre\n");
        do
        {
             /*   	if(read(csock, buffer, sizeof(buffer)) == -1)
                	{
                        	perror("read");
	                        return errno;
        	        }
                	if(strcmp(buffer, "007") == 0)
	                        continuer = false;
			else
				printf("%s\n",buffer);
	*/	lire(buffer, 20);
		if(write(csock, buffer, sizeof(buffer)) == -1)
		{
			perror("write");
			return errno;
		}
                if(strcmp(buffer, "end") == 0)
	                continuer = false;
           /*     lire(buffer, 20);
                if(write(csock, buffer, sizeof(buffer)) == -1)
                {
                        perror("write");
                        return errno;
                }
                if(strcmp(buffer, "007") == 0)
                        continuer = false;
	  */      if(read(csock, buffer, sizeof(buffer)) == -1)
                {
                        perror("read");
                        return errno;
                }
                if(strcmp(buffer, "end") == 0)
                        continuer = false;
                else
                        printf("%s\n", buffer);

        }while(continuer == true);

        close(sock);

        return 0;
}

