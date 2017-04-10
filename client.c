#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <sys/select.h>
#include <arpa/inet.h>  
#include <netinet/in.h> 
#include <unistd.h>     
#include <dirent.h>     
#include <sys/stat.h> 
#include <sys/select.h>
#include <sys/time.h>  
#include <netinet/tcp.h>

#define MAX_SIZE 2048

int max(int a, int b) {
  if (a>b)
    return a;
  else
    return b;
}

void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof;
	fd_set rset;
	char sendline[MAX_SIZE], recvline[MAX_SIZE];
	stdineof = 0; /* use for test readable, 0: connect */
	FD_ZERO(&rset);
	for ( ; ; ) {
		if (stdineof == 0) {
			FD_SET(fileno(fp), &rset);
			FD_SET(sockfd, &rset);
			maxfdp1 = max(fileno(fp), sockfd) + 1;
			select(maxfdp1, &rset, NULL, NULL, NULL);
		}
		if (FD_ISSET(sockfd, &rset)) {                           /* socket is readable */
			if (read(sockfd, recvline, MAX_SIZE) == 0) {
				if (stdineof == 1) return;
				else printf("str_cli: server terminated prematurely");
			}
			fputs(recvline, stdout);
		}
		if (FD_ISSET(fileno(fp), &rset)) {                       /* input is readable */
			if (fgets(sendline, MAX_SIZE, fp) == NULL) {      //EOF
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);               /* send FIN */
				FD_CLR(fileno(fp), &rset);
				continue;
			}
		}
		write(sockfd, sendline, strlen(sendline));
		memset(sendline, '\0', MAX_SIZE);
	}
}

int main (int argc , char **argv) {
  int cli_fd;                   
  struct sockaddr_in svr_addr;  

  int write_bytes;
  int read_bytes;               
  char buf[MAX_SIZE];           

  cli_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (cli_fd < 0) {
    perror("Create socket failed.");
    exit(1);
  }

  int KeepAlive=1;
  socklen_t KPlen=sizeof(int);
  if(setsockopt(cli_fd,SOL_SOCKET,SO_KEEPALIVE,&KeepAlive,sizeof(int))<0){
    perror("setsockopt fail\n");
    exit(1);
  } 

  bzero(&svr_addr, sizeof(svr_addr));
  svr_addr.sin_family = AF_INET;
  svr_addr.sin_port = htons(atoi(argv[2]));
  if (inet_pton(AF_INET, argv[1], &svr_addr.sin_addr) <= 0) {
     perror("Address converting fail with wrong address argument");
     return 0;
  }
  if (connect(cli_fd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0) {
    perror("Connect failed");
    exit(1);
  }

  str_cli(stdin,cli_fd);
  return 0;
}
