#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUFSIZE 30

int main(int argc, char *argv[])
{
	fd_set reads, temps;
	int result, str_len;
	char buf[BUFSIZE];
	struct timeval timeout;

	// reads�� ����
	FD_ZERO(&reads);
	FD_SET(0, &reads);		//stdin : monitoring

	while (1)
	{
		temps = reads;
		// select() �Լ��� timeout���� 5�ʷ� ����
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		result = select(1, &temps, 0, 0, &timeout);
		if (result == -1)
		{
			//printf("select() error!");
			write(1, "select() error!\n", 16);
			//fflush(stdout);
			break;
		}
		//timeout
		else if(result == 0)
		{
			write(1, "timeout!\n", 9);
		}
		//���������� select�Լ��� ����� ���
		else
		{
			if (FD_ISSET(0, &temps))
			{
				str_len = read(0, buf, BUFSIZE);
				buf[str_len] = 0;		//���ڿ��� NULL �߰�
				printf("message from console : %s", buf);
				fflush(stdout);
			}
		}
	}


	return 0;
}