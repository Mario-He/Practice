#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <zconf.h>
#include "arpa/inet.h"
#include "stdio.h"


#define PORT 6666
#define MAXLINE 4096
#define SA struct sockaddr

void str_cli(FILE *fp, int sockfd);
ssize_t readline(int fd, void *vptr, size_t maxlen);

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2) {
        printf("argc error");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    connect(sockfd, (SA*) &servaddr, sizeof(servaddr));
    str_cli(stdin, sockfd);
    exit(0);
}

void str_cli(FILE *fp, int sockfd) {
    char sendline[MAXLINE], recvline[MAXLINE];

    while (fgets(sendline, MAXLINE, fp) != NULL) {
        write(sockfd, sendline, strlen(sendline));
        if(readline(sockfd, recvline, MAXLINE) == 0) {
            printf("server terminated prematurely");
            exit(0);
        }
        fputs(recvline, stdout);
    }
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t	n, rc;
    char	c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return(0);	/* EOF, no data read */
            else
                break;		/* EOF, some data was read */
        } else
            return(-1);	/* error */
    }

    *ptr = 0;
    return(n);
}
