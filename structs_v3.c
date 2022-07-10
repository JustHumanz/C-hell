#include <stdio.h>

typedef struct{
    char * name;
    int level;
} player;

int main(){
    player humanz;
    humanz.name = "JustHumanz";
    humanz.level = 10;

    printf("Player name :%s\nPlayer level : %d",humanz.name,humanz.level);
    return 0;
}