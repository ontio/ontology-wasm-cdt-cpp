#include<stdio.h>
class MyOutstream
{
    public:
    const MyOutstream& operator << (int value)const;
    const MyOutstream& operator << (char*str)const;
};

const MyOutstream& MyOutstream::operator<<(int value)const
{
    printf("%d",value);
    return* this;
}

const MyOutstream& MyOutstream::operator<<(char* str)const
{
    printf("%s",str);
    return* this;
}

MyOutstream myout;

extern "C" {
int invoke()
{
    int a=2003;
    char* myStr="Hello,World!";
    myout << myStr << "\n";
    return 0;
}
}
