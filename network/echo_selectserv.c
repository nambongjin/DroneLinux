#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUFSIZE 30 

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}


int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	socklen_t adr_sz;
	int fd_max, fd_num, str_len, state, i;
	char buf[BUFSIZE];


	if(argc!=2)
	{
		printf("Usage : %s [port]\n", argv[0]);
		exit(1);
	}

	// 1. socket()
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	// 2. 서버 어드레스 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));


	// 3. bind()
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");
	
	// 4. listen()
	if(listen(serv_sock,5)==-1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	while(1)
	{
		cpy_reads = reads;
		timeout.tv_sec  = 5;
		timeout.tv_usec = 0;

		fd_num = select(fd_max+1,&cpy_reads,0,0,&timeout);
		// select함수가 정상적으로 호출되지 않은 경우
		if(fd_num==-1)
		{
			break;
		}
		// timeout
		else if(fd_num==0)
		{
			continue;
		}	
		// fd=3 ~ 
		for(i=3;i<fd_max+1; i++)
		{
			// connection request
			if(FD_ISSET(i, &cpy_reads))
			{
				// 서버로 접속 요청이 들어온 경우
				if(i==serv_sock)
				{
					adr_sz = sizeof(clnt_addr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_sz);
					FD_SET(clnt_sock,&reads);
					if(fd_max<clnt_sock)
						fd_max=clnt_sock;
					printf("connected client:%d\n", clnt_sock);
				}
			
				else	// 클라이언트 소켓으로 데이터 전송 요청이 들어온 경우 (fd>3)
				{
					str_len = read(i, buf, BUFSIZE);
					// 수신된 데이터가 0인 경우
					if((str_len)==0)
					{
						FD_CLR(i, &reads);
						close(i);
						printf("closed client:%d\n",i);
					}
					// 정상적으로 데이터를 수신한 경우
					else
					{
						write(i, buf, str_len);
					}
				}
			}
		} //for end
	} // while end
	close(serv_sock);
	return 0;
}		

