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
	struct sockaddr_in to_adr, from_adr;

	if(argc!=3)
	{
		printf("Usage %s [IP] [PORT]\n", argv[0]);
		exit(1);
	}

	// 1. socket()
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock==-1)
	{
		error_handling("socket() error");
	}

	// 2. 데이터를 보낼 주소(IP,PORT)설정
	memset(&to_adr, 0, sizeof(to_adr));
	to_adr.sin_family = AF_INET;
	to_adr.sin_addr.s_addr=inet_addr(argv[1]); //"192.168.1.108"
	to_adr.sin_port = htons(atoi(argv[2]));

	while(1)
	{
		printf("Insert message(q to quit):");
		fgets(message, sizeof(message), stdin);
		if((!strcmp(message,"q\n"))||(!strcmp(message,"Q\n")))
			break;

		sendto(sock, message, strlen(message), 0, \
			(struct sockaddr*)&to_adr, sizeof(to_adr));
		adr_sz = sizeof(from_adr);
		str_len = recvfrom(sock, message, sizeof(message), 0, \
			(struct sockaddr*)&from_adr, &adr_sz);
		message[str_len]=0;	//수신된 문자열끝에 NULL문자를 삽입
		printf("Message from : %s", message);
	}
	close(sock);
	return 0;
}



