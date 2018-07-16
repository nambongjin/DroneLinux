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
	int send_sock;
	struct sockaddr_in broad_adr;
	int so_brd = 1;
	FILE *fp;
	char buf[BUFSIZE];
	if(argc!=3)
	{
		printf("Usage : %s [BroadCast IP][PORT]\n", argv[0]);
		exit(1);
	}

	// 1. socket()
	send_sock = socket(PF_INET, SOCK_DGRAM, 0);
	
	// 2. 멀티캐스트 IP 및 PORT설정
	memset(&broad_adr, 0, sizeof(broad_adr));
	broad_adr.sin_family=AF_INET;
	broad_adr.sin_addr.s_addr=inet_addr(argv[1]);	//BroadCast IP
	broad_adr.sin_port=htons(atoi(argv[2]));		//BroadCast PORT

	setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd, sizeof(so_brd));

	if((fp=fopen("news.txt", "r"))==NULL)
		error_handling("fopen() error");

	while(!feof(fp))	//Broadcasting
	{
		fgets(buf, BUFSIZE, fp);
		sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&broad_adr, sizeof(broad_adr));
		sleep(2);
	}
	fclose(fp);
	close(send_sock);
	
	return 0;
}
