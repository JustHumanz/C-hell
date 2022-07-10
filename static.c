#include <stdio.h>

int counter(){
    static int counter = 0;
    counter++;
    return counter;
}

int main(){
    printf("%d\n",counter());
    printf("%d\n",counter());
    printf("%d\n",counter());
    return 0;
}