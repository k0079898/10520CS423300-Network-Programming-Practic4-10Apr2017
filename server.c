#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

# define MAXLINE 2048

void enable_keepalive(int sock) {
    int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1) exit(1);

    int idle = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1) exit(1);

    int interval = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1) exit(1);

    int maxpkt = 10;
    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1) exit(1);
}

int main(int argc,char **argv){
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char line[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	int KeepAlive=1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_KEEPALIVE, &KeepAlive, sizeof(int)) < 0){
	  perror("setsockopt fail\n");
	  exit(1);
	} 
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8764);
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	listen(listenfd,1024);
	maxfd = listenfd; /* initialize */
	maxi = -1; /* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++) client[i] = -1; /* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);	
	puts("Waiting for new connection....");
	for(;;){
		rset = allset; /* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(listenfd, &rset)) { /* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
			for (i = 0; i < FD_SETSIZE; i++){
				if (client[i] < 0) {
				client[i] = connfd; /* save descriptor */
				break;
				}
			}
			enable_keepalive(connfd);
			printf("Client is from: %s port:%d \n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
			if (i == FD_SETSIZE) printf("too many clients"); /*cannot accept clients*/
			FD_SET(connfd, &allset);             	 /* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;  	 /* for select */
			if (i > maxi) maxi = i;             	 /* max index in client[] array */
			if (--nready <= 0) continue;       	 /* no more readable descriptors*/
		}
		for (i = 0; i <= maxi; i++) {                	 /* check all clients for data */
			if ( (sockfd = client[i]) < 0) continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = read(sockfd, line, MAXLINE)) == 0) {
					/* connection closed by client */
					close(sockfd);
					printf("Client is closed\n");
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else {
					line[n] = '\0';
					printf("Client: %s", line);	
					write(sockfd, line, n);
				}
				if (--nready <= 0) break;
			}
		}
	}
}
