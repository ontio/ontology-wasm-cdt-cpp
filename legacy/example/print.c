#include<stdio.h>

int invoke()
{
	printf("hello world\n");
	int a = 10;
	int b = 20;
	char *ptr = "my name is steven";
	printf("res = %x, res = %d, str = %s\n", a, b, ptr);
	return 0;
}
