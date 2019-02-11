#include<string.h>
#include<malloc.h>

int invoke()
{
	char *ptr = (char *)malloc(8 * sizeof(char));
	ptr[7] = '9';
	memset(ptr, '3' ,8*sizeof(char));
	return ptr[7];
}
