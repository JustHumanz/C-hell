#include <stdio.h>

int recursion_v2(int x,int y){
    if (x==1) {
        return y;
    } else if (x > 1) {
        return y+recursion_v2(x-1, y);
    }

    return 0;
}

int main(){
    int x,y;
    printf("3 times 5 is %d",recursion_v2(3, 5));
    return 0;
}