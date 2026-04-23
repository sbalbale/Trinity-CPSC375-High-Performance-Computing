/* 
 * client4.c - UNIX domain, connectionless client
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
	int orig_sock, i;
	static struct sockaddr_un clnt_adr, serv_adr;
	static char buf[10], client_file[20];
	
	if ((orig_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("client: socket()");
		exit(1);
	}
	
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path, "server_socket");

	sprintf(client_file, "%07d_socket", getpid());
	clnt_adr.sun_family = AF_UNIX;
	strcpy(clnt_adr.sun_path, client_file);
	
	if (bind(orig_sock, (struct sockaddr *) &clnt_adr, sizeof(clnt_adr)) < 0) {
          //sizeof(clnt_adr.sun_family)+strlen(clnt_adr.sun_path)) < 0) {
		perror("bind error");
		exit(1);
	}

	for (i = 1; i <= 10; i++) {
		sleep(1);
		sprintf(buf, "c: %d\n", i);
		sendto(orig_sock, buf, sizeof(buf), 0, (struct sockaddr *) &serv_adr, 
		 	 sizeof(serv_adr));
	}
	exit(0);
}