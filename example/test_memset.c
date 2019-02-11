#include<string.h>
#include<malloc.h>

int invoke()
{
	int *ptr = (int *)malloc(8 * sizeof(int));
	ptr[7] = 9;
	memset(ptr, 0 ,8*sizeof(int));
	return ptr[7];
}
