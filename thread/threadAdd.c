#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

//#define DEBUG


struct Num
{
	int start;
	int end;
};


void* add_func(void* data)
{
	struct Num num1;
	int i;
	long result=0;

	num1 = *((struct Num*)data);

#ifdef DEBUG
	printf("num1.start=%d\n", num1.start);
	printf("num1.end=%d\n", num1.end);
#endif
	for(i=num1.start;i<=num1.end;i++)
	{
		result += i;
	}
	
	return (void*)(result);
}

long adder(int start, int end)
{
        int i;
        long long result=0;

        for(i = start ;i<=end; i++)
        {
                result += i;
        }

        return result;
}


int main(int argc, char *argv[])
{
	pthread_t p_thread[2];
	struct Num num1, num2;
	long sum1, sum2;
	long result;
	int threadNum;
	struct timeval StartTime, EndTime, ResultTime;	

	threadNum = atoi(argv[1]);

	
	gettimeofday(&StartTime, NULL);

	switch(threadNum)
	{
		case 1: result = adder(1, atoi(argv[2]));
			break;
		case 2:
		        num1.start = 1;
        		num2.end = (atoi(argv[2]));
        		num1.end = (num2.end>>1);
        		num2.start = num1.end+1;
			if(pthread_create(&p_thread[0], NULL, add_func, (void*)&num1)<0)
			{
				perror("thread1 create error:");
				exit(1);
			}

        		if(pthread_create(&p_thread[1], NULL, add_func, (void*)&num2)<0)
        		{
                		perror("thread2 create error:");
                		exit(2);
        		}

			pthread_join(p_thread[0],(void**)&sum1);
			pthread_join(p_thread[1],(void**)&sum2);

			result = sum1+sum2;
#ifdef DEBUG
			printf("sum1: %ld\n", sum1);
			printf("sum2: %ld\n", sum2);
#endif
			break;
		default:
			break;
	}

	gettimeofday(&EndTime, NULL);

	printf("Result : %ld\n", result);

	// 마이크로초 계산
	if(EndTime.tv_usec<StartTime.tv_usec)
	{
		EndTime.tv_sec--;
		ResultTime.tv_usec = (1000000 + EndTime.tv_usec) - StartTime.tv_usec;
	}
	else
		ResultTime.tv_usec = EndTime.tv_usec - StartTime.tv_usec;

	// 초 계산
	ResultTime.tv_sec = EndTime.tv_sec - StartTime.tv_sec;
	
	printf("Running Time : %ld.%ld\n", ResultTime.tv_sec,ResultTime.tv_usec);
	return 0;
}

