#include <stdio.h>

void humanz_fcn(int num){
    printf("This was from humanz_fcn %d \n",num);
}

int main() {
    void (*ptr) (int);
    ptr = &humanz_fcn;
    printf("This from main\n");
    ptr(5);
    printf("Back to main");
    return 0;
}