#include <stdio.h>

void new_func(){
    printf("new_func \n");
}

void new_func_para(int num,char str[]){
    printf("new_func %d - %s \n",num,str);
}

int new_func_return(int num){
    return num+10;
}

int main(){
    new_func();
    new_func_para(11,"abc");
    printf("num : %d",new_func_return(11));
    return 0;
}