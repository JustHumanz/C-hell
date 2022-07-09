#include <stdio.h>

int main(){
    int a = 10;
    printf("=======While=========\n");
    while (a < 100) {
        printf("%d\n",a);
        a+=10;
    }

    printf("=======Do=========\n");
    do {
        printf("%d\n",a);
        a -= 10;
    }while (a > 0);
    return 0;
}