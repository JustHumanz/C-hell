#include <stdio.h>

typedef struct{
    int y;
    int x;
} point;

void counter(int *num){
    (*num)++;
}

void move(point *p){
    (*p).x++;
    (*p).y++;
}

int main(){
    int num = 10;
    printf("%d\n",num);
    counter(&num);
    printf("%d\n",num);

    point posisi = {500,400};
    printf("x %d y %d\n",posisi.x,posisi.y);
    move(&posisi);
    printf("x %d y %d\n",posisi.x,posisi.y);


    return 0;
}