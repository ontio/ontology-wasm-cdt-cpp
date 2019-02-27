#include<stdio.h>
//#pragma warning(disable:4786)
#include<string>
#include<map>
using namespace std;

#define PRINTMAP(iterSuffix, mapName)\
{\
    PRJ_MAP_STRING2INT_WORDREC::iterator iter##iterSuffix = mapName.begin();\
    for (; iter##iterSuffix != mapName.end(); ++iter##iterSuffix)\
    {\
        printf("\"%s\" -> total: %d\n", iter##iterSuffix->first.c_str(), iter##iterSuffix->second);\
    }\
    printf("\n");\
}

typedef map<string, int> PRJ_MAP_STRING2INT_WORDREC;

extern "C"{
int invoke()
{
    map<string, int> mapWordRecPrep;
    PRJ_MAP_STRING2INT_WORDREC mapWordRecVerb;

    mapWordRecPrep["the"]  = 100;
    mapWordRecPrep["so"]   = 50;

    mapWordRecVerb["find"] = 1;
    mapWordRecVerb["seen"] = 2;
    mapWordRecVerb["jump"] = 3;
    mapWordRecVerb["swim"] = 4;
    mapWordRecVerb.insert(map<string, int>::value_type("look", 5));
    mapWordRecVerb.insert(pair<string, int>("walk", 6));

    printf("Insert method: value_type\n");
    pair<map<string, int>::iterator, bool> inserted;
    inserted = mapWordRecVerb.insert(map<string, int>::value_type("walk", 7));
    printf("%s\n", true == inserted.second ? "Insert success!" : "Insert failed!");
    PRINTMAP(Ver, mapWordRecVerb);

    printf("Insert method: array\n");
    mapWordRecVerb["walk"] = 7;
    PRINTMAP(Ver, mapWordRecVerb);

    //查找记录
    map<string, int>::iterator iter = mapWordRecPrep.find("so");
    printf("%s\n", iter == mapWordRecPrep.end() ? "Not find!" : "Find!");

    //删除记录
    map<string, int>::iterator iterV = mapWordRecVerb.find("seen");
    if (iterV != mapWordRecVerb.end())
    {
        mapWordRecVerb.erase(iterV);              /* 迭代器方式删除 */
        printf("\nDelete word \"seen\" done!\n");
        PRINTMAP(Verb, mapWordRecVerb);
    }

    int n = mapWordRecVerb.erase("swim");         /*  关键字方式删除 */
    printf("\nDelete word \"swim\" done!\n");
    PRINTMAP(Verb, mapWordRecVerb);

    mapWordRecVerb.erase(mapWordRecVerb.begin(), mapWordRecVerb.end()); /* 成片删除 */
    printf("\nDelete all word done!\n");
    PRINTMAP(Verb, mapWordRecVerb);

    //map的其它函数
    if (mapWordRecVerb.empty() && 0 == mapWordRecVerb.size())   /* 判断map是否为空 */
    {
        printf("mapWordRecVerb is empty!\n\n");
    }

    PRINTMAP(Pre, mapWordRecPrep)
    mapWordRecPrep.clear();                                     /* 清空map */
    PRINTMAP(Pre, mapWordRecPrep)
    if (mapWordRecPrep.empty() && 0 == mapWordRecPrep.size())
    {
        printf("mapWordRecPrep is empty!\n\n");
    }
    return 0;
}
}
