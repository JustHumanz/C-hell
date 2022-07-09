#include <stdio.h>

void tFunc();
int tFunc2(int num);

int main(){
    tFunc();
    printf("from func %d",tFunc2(10));
    return 0;
}


void tFunc(){
    printf("from test fuction\n");
}

int tFunc2(int num){
    return num+100;
}