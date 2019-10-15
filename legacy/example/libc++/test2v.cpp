#include<string.h>
#include<vector>
#include<iostream>
#include<algorithm>

using namespace std;

extern "C" {
int invoke()
{
	int N = 5, M = 6;
	vector<vector<int>> obj(N, vector<int>(M));

	for(int i = 0; i < obj.size(); i++)
	{
		for (int j = 0; j < obj[i].size(); j++)
		{
			printf("%d ", obj[i][j]);
		}
		printf("\n");
	}
	return 0;
}
}
