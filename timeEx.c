#include <stdio.h>
#include <time.h>
#include <sys/time.h>    //gettimeofday()함수에서 사용
#include <stdlib.h>


int main(int argc, char *argv[])
{
	time_t UTCtime;
	struct tm *tm;
	char buf[BUFSIZ];
	struct timeval UTCtime_u;

	// 1. time()
	time(&UTCtime);
	printf("time : %u\n", (unsigned)UTCtime);	// UTC 현재시간 출력
	
	// 2. localtime()
	tm = localtime(&UTCtime);
	printf("year:%d\n", tm->tm_year);

	// 3. asctime()
	printf("asctime: %s",asctime(tm)); //현재 시간을 tm구조체에서 읽어와서 출력

	// 4. strftime()
	strftime(buf, sizeof(buf), "%Y-%m-%e %H:%M:%S %A", tm);
	printf("strftime : %s\n", buf);


	// 5. gettimeofday()
	gettimeofday(&UTCtime_u, NULL);	    // UTC 현재시간(마이크로초까지)
	printf("gettimeofday : %ld/%ld\n", UTCtime_u.tv_sec, UTCtime_u.tv_usec);
	return 0;
}
