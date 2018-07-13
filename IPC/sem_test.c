#include <sys/types.h> 
#include <sys/sem.h> 
#include <sys/ipc.h> 
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 

#define SEMKEY 2345 

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
};

static int  semid;
int main(int argc, char **argv)
{
    FILE* fp;
    char buf[11];
    char count[11];

    union semun sem_union; 

    // open 과 close 를 위한 sembuf 구조체를 정의한다. 
    struct sembuf mysem_open  = {0, -1, SEM_UNDO}; // 세마포어 얻기
    struct sembuf mysem_close = {0, 1, SEM_UNDO};  // 세마포어 돌려주기
    int sem_num;

    memset(buf, 0x00, 11);
    memset(count, 0x00, 11);

    // 아규먼트가 있으면 생성자
    // 그렇지 않으면 소비자이다.
    if (argc > 1)
        sem_num = 1;
    else 
        sem_num = 0;            

    // 세마포설정을 한다. 
    semid = semget((key_t)234, sem_num, 0660|IPC_CREAT);
    if (semid == -1)
    {
        perror("semget error ");
        exit(0);
    }    

	// 세마포어 초기화
        sem_union.val = 1;
        if ( -1 == semctl( semid, 0, SETVAL, sem_union))
        {   
                printf( "semctl()-SETVAL 실행 오류\n");
                return -1; 
        }   
    // counter.txt 파일을 열기 위해서 세마포어검사를한다. 
    if(semop(semid, &mysem_open, 1) == -1)
    {
        perror("semop error ");
        exit(0);
    }

    if ((fp = fopen("counter.txt", "r+")) == NULL)
    {
        perror("fopen error ");
        exit(0);
    }
    // 파일의 내용을 읽은후 파일을 처음으로 되돌린다.  
    fgets(buf, 11, fp);
    rewind(fp);

    // 개행문자를 제거한다. 
    buf[strlen(buf) - 1] = 0x00;

    sprintf(count, "%d\n", atoi(buf) + 1); 
    printf("%s", count);
    // 10초를 잠들고 난후 count 를 파일에 쓴다. 
    sleep(10);
    fputs(count,fp);

    fclose(fp);
    // 모든 작업을 마쳤다면 세마포어 자원을 되돌려준다
    semop(semid, &mysem_close, 1);
    return 1;
}
