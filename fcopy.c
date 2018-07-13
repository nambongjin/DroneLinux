#include <stdio.h>


int main(int argc, char **argv)
{
	int count;
	int temp;
	FILE *in, *out;
	char buf[BUFSIZ];

	printf("FILE* in addr = %p\n", in);

	if(argc<3)
	{
		fprintf(stderr, "Usage : fcopy file1 file2\n"); 
		return -1;
	}

	// 원본 파일 열기
	if((in = fopen(argv[1], "r"))==NULL)
	{
		perror(argv[1]);
		return -1;
	}

	printf("FILE* in addr = %p\n", in);

	temp = in->_fileno;
	printf("fd=%d\n",temp);
	// 대상 파일 열기
	if((out = fopen(argv[2], "w"))==NULL)
	{
		perror(argv[2]);
		return -1;
	}

	// 원본파일의 내용을 읽어서 대상파일에 쓰기
	while((count = fread(buf, sizeof(char), BUFSIZ, in)) >0)
		fwrite(buf, sizeof(char), count, out);

	fclose(out);
	fclose(in);

	return 0;

}
