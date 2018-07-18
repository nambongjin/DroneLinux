#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

struct Num
{
	int start;
	int end;
};

long long result = 0;
pthread_mutex_t mutex;

void add_func(void* data)
{
	struct Num num1;
	int i;

	num1 = *((struct Num*)data);

	for(i=num1.start;i<=num1.end;i++)
	{
		pthread_mutex_lock(&mutex);
		result += i;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}


int main()
{
	pthread_t p_thread[2];
	struct Num num1, num2;
	struct timeval StartTime, EndTime, ResultTime;	

	pthread_mutex_init(&mutex, NULL);

	num1.start = 1;
	num2.end = 20000;
	num1.end = 10000;
	num2.start = 10001;
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

	pthread_join(p_thread[0],NULL);
	pthread_join(p_thread[1],NULL);

	printf("Result : %ld\n", result);

	return 0;
}

