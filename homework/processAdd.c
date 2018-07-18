#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define dataCount 1
#define SHMSIZE  (sizeof(long long) * dataCount)
#define KEY_VALUE       (key_t)1234

struct Num
{
	int start;
	int end;
};

struct Num num;
long long result = 0;
void *shared_mem = (void*)0;  //NULL포인터로 초기화
int shmid;
long long *shmaddr;	// 실제 데이터 영역(공유메모리)을 접근하기 위한 포인터

void sigHandler(int sig)
{
	int i;
	num.start = 10001;
	num.end = 20000;

	for (i = num.start; i <= num.end; i++)
	{
		result += i;
	}

	result += *(shmaddr);

	// STEP 4. shmdt
	if (shmdt(shared_mem) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	// STEP 5. shmclt
	if (shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Result : %ld\n", result);

	(void)signal(SIGINT, SIG_DFL);
}

int main(void)
{
	int status;
	pid_t pid;

	int i, s, sig;
	
	// STEP 1. shmget
	shmid = shmget(KEY_VALUE, SHMSIZE, 0666 | IPC_CREAT);
	if (shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// STEP 2. shmat
	shared_mem = shmat(shmid, (void*)0, 0);
	if (shared_mem == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	// STEP 3. shared memory access
	shmaddr = (long long*)shared_mem;

	pid = fork();

	// fork()를 실행했는데 
	// 정상적으로 자식프로세스가 생성되지 않은 경우
	if(pid<0)
	{
		printf("Error: fork()\n");
		return -1;
	}
	// 자식 프로세스인 경우 실행
	else if(pid==0)
	{
		num.start = 1;
		num.end = 10000;

		for (i = num.start; i <= num.end; i++)
		{
			result += i;
		}

		*shmaddr = result;

		// STEP 4. shmdt
		if (shmdt(shared_mem) == -1)
		{
			fprintf(stderr, "shmdt failed\n");
			exit(EXIT_FAILURE);
		}

		sig = SIGINT;

		s = kill(getppid(), sig);

		if (sig != 0)
		{
			// 정상적으로 kill함수가 호출되지 않은 경우
			if (s == -1)
				printf("Error:system call kill()\n");

			// 정상적으로 kill함수가 호출된 경우
			//else if (s == 0)
			//	printf("Process exists and we can send it a signal\n");
		}

		return 0;
	}
	// 부모 프로세스인 경우 실행
	else
	{
		signal(SIGINT, sigHandler);
	}

	pid = wait(&status);
		
	return 0;
}
