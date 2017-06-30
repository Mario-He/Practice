#include "zconf.h"
#include "strings.h"
#include "arpa/inet.h"
#include "stdlib.h"
#include "stdio.h"
#include "sys/errno.h"
#include "sys/signal.h"
#include "sys/wait.h"

#define PORT 6666
#define LISTENQ 1024
#define MAXLINE 4096
#define SA struct sockaddr

void str_echo(int sockfd);

int main(int argc, char **argv) {
    int     sockfd, connfd;
    socklen_t clilen;
    pid_t   childpid;
    struct  sockaddr_in servaddr, cliaddr;
    void    sig_chld(int);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create sockfd failed!");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
        printf("bind sockfd failed!");
        exit(0);
    }

    if(listen(sockfd, LISTENQ) < 0){
        printf("listen sockfd failed!");
        exit(0);
    }

    signal(SIGCHLD, sig_chld);

    for( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = accept(sockfd, (SA *) &cliaddr, &clilen);
        if ( (childpid = fork()) == 0) {	/* child process */
            close(sockfd);	    /* close listening socket */
            str_echo(connfd);	/* process the request */
            exit(0);
        }
        close(connfd);
    }
}

void str_echo(int sockfd) {
    ssize_t n;
    char buf[MAXLINE];

    again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        write(sockfd, buf, n);
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0){
        printf("str_echo: read error");
        exit(0);
    }
}

void sig_chld(int signo) {
    pid_t pid;
    int stat;

    pid = wait(&stat);
    printf("child %d terminated\n", pid);
    return;
}
