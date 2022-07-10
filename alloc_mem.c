#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char * nickname;
    int level;
} player;

int main(){
    player * humanz;
    humanz = (player *)malloc(sizeof(player));
    humanz->level = 10;
    humanz->nickname = "JustHumanz";
    printf("Nick : %s\nLevel : %d",humanz->nickname,humanz->level);
    free(humanz);
    return 0;
}
