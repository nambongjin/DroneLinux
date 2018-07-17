#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUFSIZE	100
#define EPOLL_SIZE 50

#define DEBUG

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
	socklen_t adr_sz;
	int str_len, i;
	char buf[BUFSIZE];

	// epoll관련
	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	if(argc!=2)
	{
		printf("Usage : %s [port]\n", argv[0]);
		exit(1);
	}

	// 1.socket()
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	// 2. 서버 주소값 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	// 3. bind()
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");
	
	// 4. listen()
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	// epoll : 1. epoll_create()
	epfd = epoll_create(EPOLL_SIZE);	// epoll이벤트 구조체를 50개 생성
	ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
 	if(ep_events==NULL)
		error_handling("epoll event malloc error");

#ifdef DEBUG
	printf("epfd=%d\n", epfd);
#endif

	// epoll : 2. serv_sock 이벤트 등록
	// epoll_ctl() : EPOLL_CTL_ADD
	event.events  = EPOLLIN;
	event.data.fd = serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);
	
	while(1)
	{
		// epoll : 3. epoll_wait()
		event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1); 
		// epoll_wait()함수 호출 에러 발생시
		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}

		for(i=0;i<event_cnt;i++)
		{
			// 클라이언트로부터 서버에대한 접속 요청이 발생한 경우
			if(ep_events[i].data.fd==serv_sock)
			{
				adr_sz = sizeof(clnt_addr);
				clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_sz);

#ifdef DEBUG	
				printf("clnt_sock=%d\n",clnt_sock);
#endif

				// 클라이언트가 접속할때마다 epoll이벤트에 등록
			    event.events  = EPOLLIN;
    			event.data.fd = clnt_sock;
    			epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
			}
			// 클라이언트로 부터 들어오는 입력 데이터 처리
			else
			{
				str_len=read(ep_events[i].data.fd, buf, BUFSIZE);
				if(str_len==0)	// EOF인 경우 : close, half-close인 경우
				{
					// 클라이언트 접속을 종료하기전 epoll_event를 해제
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
					printf("closed client:%d\n",ep_events[i].data.fd);
				}
				// 클라이언트로부터 입력된 데이터를 echo로 되돌림
				else
				{
					write(ep_events[i].data.fd, buf, str_len);
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	
	return 0;
}
