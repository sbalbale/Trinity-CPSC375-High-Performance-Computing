/* 
 *  server4.c - UNIX domain, connectionless server
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(void)
{
	int orig_sock, clnt_len, i;
	static struct sockaddr_un clnt_adr, serv_adr;
	static char buf[10];
	
	unlink("server_socket");
	if ((orig_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("server: socket()");
		exit(1);
	}
	
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path, "server_socket");
	
	if (bind(orig_sock, (struct sockaddr *) &serv_adr, sizeof(struct sockaddr_un)) < 0) {
		perror("server: bind()");
		exit(1);
	}

	for (i = 1; i <= 10; i++) {
		recvfrom(orig_sock, buf, sizeof(buf), 0, (struct sockaddr *) 
                     &clnt_adr, &clnt_len);
		printf("s-> %s", buf);
	}
	exit(0);
}