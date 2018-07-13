#define _CRT_SECURE_NO_WARNINGS    // fopen 보안 경고로 인한 컴파일 에러 방지
#include <stdio.h>    // fopen, fread, fseek, fprintf, fclose 함수가 선언된 헤더 파일
#include <stdlib.h>   // malloc, free 함수가 선언된 헤더 파일
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "bitmap.h"

//#define DEBUG

#pragma pack(push, 1)                // 구조체를 1바이트 크기로 정렬


typedef struct _RGBTRIPLE            // 24비트 비트맵 이미지의 픽셀 구조체
{
    unsigned char rgbtBlue;          // 파랑
    unsigned char rgbtGreen;         // 초록
    unsigned char rgbtRed;           // 빨강
} RGBTRIPLE;

#pragma pack(pop)

#define PIXEL_SIZE   3    // 픽셀 한 개의 크기 3바이트(24비트)
#define PIXEL_ALIGN  4    // 픽셀 데이터 가로 한 줄은 4의 배수 크기로 저장됨

int main()
{
    FILE *fpTxt;                    // 텍스트 파일 포인터

    int size;                // 픽셀 데이터 크기
    int width, height;       // 비트맵 이미지의 가로, 세로 크기
    int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기

    //이미지 크기정보를 전달하기 위한 포인터와 id선언
    void *header_mem = (void*)0;
    int headerid;
    int *headeraddr;

    void *shared_mem = (void*)0;  //NULL포인터로 초기화
    int shmid;
    unsigned char *shmaddr;   // 실제 데이터 영역(공유메모리)을 접근하기 위한 포인터


    // 각 픽셀을 표현할 ASCII 문자. 인덱스가 높을 수록 밝아지는 것을 표현
    char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' ' };   // 11개

    // STEP 1. header : shmget
    headerid = shmget(KEY_VALUE2, HEADERSIZE, 0666 | IPC_CREAT);
    if(shmid==-1)
    {
        fprintf(stderr,"header: shmget failed\n");
        exit(EXIT_FAILURE);
    }

    // STEP 2. header : shmat
    header_mem = shmat(headerid,(void*)0, 0);
    if(shared_mem==(void*)-1)
    {
        fprintf(stderr, "header: shmat failed\n");
        exit(EXIT_FAILURE);
    }
    // STEP 3. header : shared memory access
    headeraddr = (int*)header_mem;
    size   = *(headeraddr+0);
    width  = *(headeraddr+1);
    height = *(headeraddr+2);
    padding= *(headeraddr+3);

    // STEP 4. shmdt
    if(shmdt(header_mem)==-1)
    {
        fprintf(stderr, "header: shmdt failed\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    printf("size=%d\n",size);
    printf("width=%d\n",width);
    printf("height=%d\n",height);
    printf("padding=%d\n",padding);
#endif

    // STEP 1. shmget
    shmid = shmget(KEY_VALUE, size, 0666 | IPC_CREAT);
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
    shmaddr = (char*)shared_mem;


    fpTxt = fopen("ascii.txt", "w");    // 결과 출력용 텍스트 파일 열기
    if (fpTxt == NULL)    // 파일 열기에 실패하면
    {
        return 0;         // 프로그램 종료
    }

    // 픽셀 데이터는 아래 위가 뒤집혀서 저장되므로 아래쪽부터 반복
    // 세로 크기만큼 반복
    for (int y = height - 1; y >= 0; y--)
    {
        // 가로 크기만큼 반복
        for (int x = 0; x < width; x++)
        {
            // 일렬로 된 배열에 접근하기 위해 인덱스를 계산
            // (x * 픽셀 크기)는 픽셀의 가로 위치
            // (y * (세로 크기 * 픽셀 크기))는 픽셀이 몇 번째 줄인지 계산
            // 남는 공간 * y는 줄별로 누적된 남는 공간
            int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);

            // 현재 픽셀의 주소를 RGBTRIPLE 포인터로 변환하여 RGBTRIPLE 포인터에 저장
            RGBTRIPLE *pixel = (RGBTRIPLE *)&shmaddr[index];

            // RGBTRIPLE 구조체로 파랑, 초록, 빨강값을 가져옴
            unsigned char blue = pixel->rgbtBlue;
            unsigned char green = pixel->rgbtGreen;
            unsigned char red = pixel->rgbtRed;

            // 파랑, 초록, 빨강값의 평균을 구하면 흑백 이미지를 얻을 수 있음
            unsigned char gray = (red + green + blue) / PIXEL_SIZE;

            // 흑백값에 ASCII 문자의 개수를 곱한 뒤 256으로 나누면 흑백값에 따라 
            // ASCII 문자의 인덱스를 얻을 수 있음
            char c = ascii[gray * sizeof(ascii) / 256];

            // 비트맵 이미지에서는 픽셀의 가로, 세로 크기가 똑같지만
            // 보통 ASCII 문자는 세로로 길쭉한 형태이므로 정사각형 모양과 비슷하게 보여주기 위해
            // 같은 문자를 두 번 저장해줌
            fprintf(fpTxt, "%c%c", c, c);    // 텍스트 파일에 문자 출력
        }

        fprintf(fpTxt, "\n");    // 가로 픽셀 저장이 끝났으면 줄바꿈을 해줌
    }

    // STEP 4. shmdt
    if(shmdt(shared_mem)==-1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    printf("shmaddr: read done!\n");
    fclose(fpTxt);    // 텍스트 파일 닫기

    return 0;
}


