#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}

int main(int argc, char *argv[])
{
	int tcp_sock, udp_sock;
	int sock_type;
	socklen_t optlen;
	int state;

	optlen = sizeof(sock_type);

	// TCP socket생성
	tcp_sock = socket(PF_INET,SOCK_STREAM,0);

	// UDP socket생성
	udp_sock = socket(PF_INET,SOCK_DGRAM, 0);

	printf("SOCK_STREAM %d\n", SOCK_STREAM);
	printf("SOCK_DGRAM %d\n", SOCK_DGRAM);

	// TCP getsockopt()
	state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
	if(state)
		error_handling("tcp getsockopt() error");
	printf("Socket type one : %d\n", sock_type);
	
        // UDP getsockopt()
        state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
        if(state)
                error_handling("udp getsockopt() error");
        printf("Socket type two : %d\n", sock_type);

	return 0;
}
 
