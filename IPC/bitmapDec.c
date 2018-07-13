#define _CRT_SECURE_NO_WARNINGS    // fopen 보안 경고로 인한 컴파일 에러 방지
#include <stdio.h>    // fopen, fread, fseek, fprintf, fclose 함수가 선언된 헤더 파일
#include <stdlib.h>   // malloc, free 함수가 선언된 헤더 파일
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "bitmap.h"


#pragma pack(push, 1)                // 구조체를 1바이트 크기로 정렬

typedef struct _BITMAPFILEHEADER     // BMP 비트맵 파일 헤더 구조체
{
    unsigned short bfType;           // BMP 파일 매직 넘버
    unsigned int   bfSize;           // 파일 크기
    unsigned short bfReserved1;      // 예약
    unsigned short bfReserved2;      // 예약
    unsigned int   bfOffBits;        // 비트맵 데이터의 시작 위치
} BITMAPFILEHEADER;

typedef struct _BITMAPINFOHEADER     // BMP 비트맵 정보 헤더 구조체(DIB 헤더)
{
    unsigned int   biSize;           // 현재 구조체의 크기
    int            biWidth;          // 비트맵 이미지의 가로 크기
    int            biHeight;         // 비트맵 이미지의 세로 크기
    unsigned short biPlanes;         // 사용하는 색상판의 수
    unsigned short biBitCount;       // 픽셀 하나를 표현하는 비트 수
    unsigned int   biCompression;    // 압축 방식
    unsigned int   biSizeImage;      // 비트맵 이미지의 픽셀 데이터 크기
    int            biXPelsPerMeter;  // 그림의 가로 해상도(미터당 픽셀)
    int            biYPelsPerMeter;  // 그림의 세로 해상도(미터당 픽셀)
    unsigned int   biClrUsed;        // 색상 테이블에서 실제 사용되는 색상 수
    unsigned int   biClrImportant;   // 비트맵을 표현하기 위해 필요한 색상 인덱스 수
} BITMAPINFOHEADER;

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
    FILE *fpBmp;                    // 비트맵 파일 포인터
    BITMAPFILEHEADER fileHeader;    // 비트맵 파일 헤더 구조체 변수
    BITMAPINFOHEADER infoHeader;    // 비트맵 정보 헤더 구조체 변수

    // 이미지 사이즈 정보를 저장하기 위한 공유 메모리 변수
    void *header_mem = (void*)0;
    int headerid;
    int *headeraddr;

    // 이미지 데이터를 저장하기 위한 공유 메모리 변수
    void *shared_mem = (void*)0;  //NULL포인터로 초기화
    int shmid;
    char *shmaddr;   // 실제 데이터 영역(공유메모리)을 접근하기 위한 포인터

    int size;                // 픽셀 데이터 크기
    int width, height;       // 비트맵 이미지의 가로, 세로 크기
    int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기

    // 각 픽셀을 표현할 ASCII 문자. 인덱스가 높을 수록 밝아지는 것을 표현
    char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' ' };   // 11개

    fpBmp = fopen("lena.bmp", "rb");    // 비트맵 파일을 바이너리 모드로 열기
    if (fpBmp == NULL)    // 파일 열기에 실패하면
        return 0;         // 프로그램 종료

    // 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    // 매직 넘버가 MB가 맞는지 확인(2바이트 크기의 BM을 리틀 엔디언으로 읽었으므로 MB가 됨)
    // 매직 넘버가 맞지 않으면 프로그램 종료
    if (fileHeader.bfType != 'MB')
    {
        fclose(fpBmp);
        return 0;
    }

    // 비트맵 정보 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    // 24비트 비트맵이 아니면 프로그램 종료
    if (infoHeader.biBitCount != 24)
    {
        fclose(fpBmp);
        return 0;
    }

    size = infoHeader.biSizeImage;    // 픽셀 데이터 크기
    width = infoHeader.biWidth;       // 비트맵 이미지의 가로 크기
    height = infoHeader.biHeight;     // 비트맵 이미지의 세로 크기

    // 이미지의 가로 크기에 픽셀 크기를 곱하여 가로 한 줄의 크기를 구하고 4로 나머지를 구함
    // 그리고 4에서 나머지를 빼주면 남는 공간을 구할 수 있음.
    // 만약 남는 공간이 0이라면 최종 결과가 4가 되므로 여기서 다시 4로 나머지를 구함
    padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * PIXEL_SIZE + padding) * height;
    }


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
    
    *(headeraddr+0) = size;
    *(headeraddr+1) = width;
    *(headeraddr+2) = height;
    *(headeraddr+3) = padding;

    // STEP 4. shmdt
    if(shmdt(header_mem)==-1)
    {
        fprintf(stderr, "header: shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    // STEP 1. imageData : shmget
    shmid = shmget(KEY_VALUE, size, 0666 | IPC_CREAT);
    if(shmid==-1)
    {
        fprintf(stderr,"shmget failed\n");
        exit(EXIT_FAILURE);
    }

    // STEP 2. imageData : shmat
    shared_mem = shmat(shmid,(void*)0, 0);
    if(shared_mem==(void*)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    // STEP 3. imageData : shared memory access
    shmaddr = (char*)shared_mem;


    // 파일 포인터를 픽셀 데이터의 시작 위치로 이동
    fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

    // 파일에서 픽셀 데이터 크기만큼 읽음. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(shmaddr, size, 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    fclose(fpBmp);    // 비트맵 파일 닫기

    // STEP 4. shmdt
    if(shmdt(shared_mem)==-1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    printf("shmaddr: write done!\n");

    return 0;
}


