#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	int status;
	pid_t pid;

	// 자신의 PID값을 확인한다.
	printf("Process ID = %d\n", getpid());
	// 부모의 PID값을 확인한다.
	printf("Parent Process ID = %d\n", getppid());
	
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
		printf("Child: Hello!!\n");
		return 0;
	}
	// 부모 프로세스인 경우 실행
	else
	{
		printf("Parent: Hi!!!\n");
		printf("Child PID : %d\n",pid);
	}

	pid = wait(&status);
	printf("EXIT:Child pid=%d\n",pid);
		
	return 0;
}
