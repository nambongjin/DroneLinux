#include <unistd.h>   
#include <stdio.h> 
#include <stdlib.h>  

int main()   
{
	printf("Running ps with system\n");    
	system("ps -ax");   
	printf("Done.\n");    
	return 5;   
}
