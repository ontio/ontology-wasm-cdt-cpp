#include<stdio.h>
#include<vector>
#include <iostream>
#include<algorithm>
#include<string.h>

using namespace std;

extern "C" {
int invoke()
{
	int i = 0;
    	vector<int> v;

	for( i = 0; i < 10; i++ )
	{
		v.push_back(i);
	}

	for( i = 0; i < v.size(); i++ )
	{
		//cout << v[i] << "  ";
		printf("%d ", v[i]);

	}

	printf("\n");


	printf("反序，从大到小\n");
	reverse(v.begin(), v.end());

	for (int i=0; i< v.size(); i++)
	{
		printf("%d ", v[i]);
	}

	printf("\n");

	printf("利用迭代器\n");
	vector<int>::iterator it;
	for (it = v.begin(); it != v.end(); it++)
	{
		printf("%d ", *it);
	}
	printf("\n");

	return 0;
} 
}
