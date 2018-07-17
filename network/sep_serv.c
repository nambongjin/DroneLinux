#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE	1024

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char buf[BUFSIZE]={0,};
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_adr_sz;
	FILE *readfp;
	FILE *writefp;
	int cpy_sock;

	// 1. socket
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	// 2. 서버 주소 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	// 3. bind()
	bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	// 4. listen()
	listen(serv_sock, 5);

	clnt_adr_sz=sizeof(clnt_addr);
	// 5. accept()
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_adr_sz);

	cpy_sock = dup(clnt_sock);
	readfp = fdopen(clnt_sock, "r");
	writefp = fdopen(cpy_sock,"w");

	fputs("From Server: Hi~ client\n", writefp);
	fputs("I love all of the world\n", writefp);
	fputs("You are awesome!\n",writefp);
	fflush(writefp);
	fclose(writefp);
	shutdown(clnt_sock, SHUT_WR);
	//fclose(writefp);

	fgets(buf,sizeof(buf), readfp);
	fputs(buf, stdout);
	fclose(readfp);
	return 0;
}
