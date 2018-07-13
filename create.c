#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE	  100

int main(int argc, char *argv[])
{
	int fd;
	int count;
	char wbuf[BUFSIZE];
	char rbuf[BUFSIZE];
	char result[BUFSIZE];
        printf("argv[0]=%s\n",argv[0]);
        printf("argv[1]=%s\n",argv[1]);

	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, \
		S_IRWXU | S_IWGRP | S_IRGRP | S_IROTH);

	if(fd==-1)
	{
		printf("file error!!\n");
		return -1;
	}

	// Do not count your chickens before they hatch.
	sprintf(wbuf,"Do not count chickens before they hatch.");
	printf("%s\n",wbuf);
	count = write(fd, wbuf, strlen(wbuf));
	if(count<1)
	{
		printf("file write error!!\n");
		return -1;
	}

	// 커서 위치를 파일의 시작점 기준으로 offset 0으로 변경
	lseek(fd, 13, SEEK_SET);

	// rbuf 배열을 0으로 초기화
	memset(rbuf,0,BUFSIZE);
	count = read(fd, rbuf, BUFSIZE);
	if(count<1)
	{
		printf("file read error!!\n");
		return -1;
	}
	printf("%s\n",rbuf);

	//========
	sprintf(result,"your %s",rbuf);
	
	lseek(fd, 13, SEEK_SET);
	count = write(fd, result, strlen(result));
	if(count<1)
        {
                printf("file write error!!\n");
                return -1;
        }
	//=========
	close(fd);

}

