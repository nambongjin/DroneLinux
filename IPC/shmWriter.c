#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"

int main(void)
{
	void *shared_mem = (void*)0;  //NULL포인터로 초기화
	int shmid;
	int *shmaddr;	// 실제 데이터 영역(공유메모리)을 접근하기 위한 포인터
	int i;

	// STEP 1. shmget
	shmid = shmget(KEY_VALUE, SHMSIZE, 0666 | IPC_CREAT);
	if(shmid==-1)
	{
		fprintf(stderr,"shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// STEP 2. shmat
	shared_mem = shmat(shmid,(void*)0, 0);
	if(shared_mem==(void*)-1)
	{
                fprintf(stderr, "shmat failed\n");
                exit(EXIT_FAILURE);
	}

	// STEP 3. shared memory access
	shmaddr = (int*)shared_mem;

	for(i=0;i<dataCount;i++)
	{
		*(shmaddr+i) = i+1;
		printf("shmaddr:%p, data:%d\n", shmaddr+i,*(shmaddr+i));
	}

	// STEP 4. shmdt
	if(shmdt(shared_mem)==-1)
	{
                fprintf(stderr, "shmdt failed\n");
                exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
