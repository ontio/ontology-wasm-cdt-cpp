#include<stack>  
#include<stdio.h>
using namespace std;

extern "C"{
int invoke(void)  
{  
    stack<unsigned int>s;
    for(int i=0;i<10;i++)  
        s.push(i);  
    while(!s.empty())  
    {  
        printf("%d\n",s.top());  
        s.pop();  
    }  
    //cout<<"栈内的元素的个数为："<<s.size()<<endl;  
    printf("栈内的元素的个数为：");
    printf("%d\n", s.size());

    return 0;  
}
}
