#include <stdio.h>
#include <string.h>

struct MyStruct {
    int num;
    char name[30];
};

int main(){
    struct MyStruct s1;
    strcpy(s1.name, "aaaa");

    s1.num = 12;

    printf("str %s num %d",s1.name,s1.num);
    
    return 0;
}