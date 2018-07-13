#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUFSIZE	1024

// int main(int argc, char **argv)
int main(int argc, char *argv[])
{
	int count, fdIn, fdOut;
	char buf[BUFSIZE];

	// 인자값이 3개가 아닌 경우
	if(argc <3)
	{
		//printf("Usage : copy file1 file2\n");
		//fflush(stdout);
		write(1, "Usage : copy file1 file2\n",26);
		return -1; 
	}

	// 원본 파일이 정상적으로 열리지 않은 경우
	if((fdIn = open(argv[1], O_RDONLY)) <0)
	{
		perror(argv[1]);
		return -1;
	}

	// 대상 파일이 정상적으로 열리지 않은 경우
	if((fdOut = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR))<0)
	{
		perror(argv[2]);
		return -1;
	}

	while((count = read(fdIn, buf, sizeof(buf))) > 0)
		write(fdOut, buf, count);
	
	sleep(10);

	close(fdIn);
	close(fdOut);

	return 0;
}
