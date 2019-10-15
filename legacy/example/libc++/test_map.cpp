#include <map>  
#include <string>  
//#include <iostream>  
using namespace std;  
  
extern "C" {
int invoke()  
{  
    map<int, string> mapStudent;  
    mapStudent[1] = "student_one";  
    mapStudent[2] = "student_two";  
    mapStudent[3] = "student_three";  
    map<int, string>::iterator iter;  
  
    for(iter = mapStudent.begin(); iter != mapStudent.end(); iter++)  
    {
        //cout<<iter->first<<' '<<iter->second<<endl;  
    	printf("%d ", iter->first);
    	printf("%s ", iter->second.c_str());
	printf("over\n");
    }

    return 0;
}  
}
