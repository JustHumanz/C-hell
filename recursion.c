#include <stdio.h>

int sum(int a);

int main(){
    printf("%d",sum(1));
    return 0;
}

int sum(int a){
    if (a < 100) {
        return a+ sum(a +1);
    } else{
        return 0;
    }
}