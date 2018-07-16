#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define BUFSIZE 30

int acpt_sock;
int recv_sock;

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}


void urg_handler(int signo)
{
	int str_len;
	char buf[BUFSIZE];
	str_len = recv(recv_sock, buf, sizeof(buf)-1, MSG_OOB);
	buf[str_len]=0;
	printf("Urgent message: %s\n", buf);

}


// server구현
int main(int argc, char *argv[])
{
	struct sockaddr_in recv_addr, serv_addr;
	int str_len, state;
	socklen_t serv_adr_sz;
	struct sigaction act;
	char buf[BUFSIZE];

	if(argc!=2)
	{
		printf("Usage : %s [port]\n", argv[0]);
		exit(1);
	}

	// 시그널 구조체 초기화
	act.sa_handler=urg_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;

	// 시그널 구조체 등록
	state = sigaction(SIGURG, &act, 0);

	// 1. socket()
	acpt_sock=socket(PF_INET, SOCK_STREAM, 0);

	// 2. 서버의 주소값 설정
	memset(&recv_addr, 0, sizeof(recv_addr));
	recv_addr.sin_family=AF_INET;
	recv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	recv_addr.sin_port=htons(atoi(argv[1]));

	// 3. bind()
	if(bind(acpt_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr))==-1)
	{
		error_handling("bind() error");
	}

	// 4. listen()
	listen(acpt_sock, 5);


	serv_adr_sz=sizeof(serv_addr);
	// 5. accept()
	recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_addr, &serv_adr_sz);
	fcntl(recv_sock, F_SETOWN, getpid());

	while(str_len=recv(recv_sock, buf, sizeof(buf), 0)!=0)
	{
		if(str_len==-1)
			continue;
		buf[str_len]=0;
		printf("%s\n", buf);
	}
	close(recv_sock);
	close(acpt_sock);
	return 0;
}
