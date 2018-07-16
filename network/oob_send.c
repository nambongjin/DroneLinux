#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSIZE	30

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}


int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in recv_addr;

	if(argc!=3)
	{
		printf("Usage : %s [IP] [PORT] \n", argv[0]);
		exit(1);
	}	

	// 1. socket()
	sock = socket(PF_INET, SOCK_STREAM,0);
	
	// 2. serv_addr설정
	memset(&recv_addr, 0, sizeof(recv_addr));
	recv_addr.sin_family=AF_INET;
	recv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	recv_addr.sin_port=htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&recv_addr, sizeof(recv_addr))==-1)
		error_handling("connect() error!");
		
	// write와 send를 뒤섞어서 전송할시 우선순위에 대한 테스트
	write(sock, "123", strlen("123"));		// NORMAL DATA TRANSFER
	send(sock,"4", strlen("4"), MSG_OOB);	// OOB DATA TRANSFER
	write(sock,"567", strlen("567"));
	send(sock,"890", strlen("890"),MSG_OOB);
	close(sock);
	return 0;
}
