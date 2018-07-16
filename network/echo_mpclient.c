#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE	50

void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[])
{
	int sock, pid;
	struct sockaddr_in serv_addr;
	char message[BUFSIZE];
	int str_len;

	if(argc!=3)
	{
		printf("Usage : %s [IP] [PORT]\n", argv[0]);
		exit(1);
	}

	// 1.socket()
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)
	{
		error_handling("socket() error");
	}

	// 2. 서버의 주소(ip, port)값을 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

	// 3. connect()
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		error_handling("connect() error");
	}
	else
		printf("Connected.....\n");

	//read동작과 write동작을 동시에 진행하기 위해 별도의 프로세스로 분리
	pid = fork();

	// 자식 프로세스(write프로세스)
	if (pid == 0)
	{
		write_routine(sock, message);
	}
	// 부모 프로세스(read프로세스)
	else
	{
		read_routine(sock, message);
	}
	// 5. close()
	close(sock);
	
	return 0;
}

void read_routine(int sock, char *buf)
{
	int str_len;
	while (1)
	{
		str_len = read(sock, buf, BUFSIZE);
		if (str_len == 0)
			return;
		buf[str_len] = 0;		//NULL 문자 삽입
		printf("Message from server : %s", buf);
	}
}

void write_routine(int sock, char *buf)
{
	while (1)
	{
		fgets(buf, BUFSIZE, stdin);
		if ((!strcmp(buf, "q\n")) || (!strcmp(buf, "Q\n")))
		{
			shutdown(sock, SHUT_WR);
			return;
		}
		write(sock, buf, strlen(buf));
	}
}

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}