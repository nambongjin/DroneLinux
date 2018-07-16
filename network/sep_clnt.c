#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE	1024
int main(int argc, char *argv[])
{
	int sock;
	char buf[BUFSIZE];
	struct sockaddr_in serv_addr;

	FILE *readfp;
	FILE *writefp;
	int cpy_sock;

	// 1. socket
	sock = socket(PF_INET, SOCK_STREAM, 0);

	// 2. 서버 주소 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

	cpy_sock = dup(sock);
	// 3. connect
	connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	readfp = fdopen(sock, "r");
	writefp = fdopen(cpy_sock,"w");

	while(1)
	{
		if(fgets(buf,sizeof(buf),readfp)==NULL)
			break;
		fputs(buf, stdout);
		fflush(stdout);
	}
	fputs("From CLIENT: Thank you!\n", writefp);
	fflush(writefp);
	fclose(writefp);
	fclose(readfp);
	return 0;
}
