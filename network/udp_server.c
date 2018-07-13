#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 30

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}
    

int main(int argc, char *argv[])
{
	int sock;
	char message[BUFSIZE];
	int str_len;
	socklen_t adr_sz;
	struct sockaddr_in serv_adr, from_adr;

	if(argc!=2)
	{
		printf("Usage %s [PORT]\n", argv[0]);
		exit(1);
	}

	// 1. socket()
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock==-1)
	{
		error_handling("socket() error");
	}

	// 2. 데이터를 보낼 주소(IP,PORT)설정
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); 
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	while(1)
	{
                adr_sz = sizeof(from_adr);
                str_len = recvfrom(sock, message, sizeof(message), 0, \
                        (struct sockaddr*)&from_adr, &adr_sz);

		sendto(sock, message, strlen(message), 0, \
			(struct sockaddr*)&from_adr, adr_sz);
	}
	close(sock);
	return 0;
}



