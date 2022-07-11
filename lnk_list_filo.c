#include <stdio.h>
#include <stdlib.h>

typedef struct player{
    char * name;
    int level;
    struct player * players;
}player_t;

void print_player(player_t * players){
    player_t * current = players;

    while (current != NULL) {
        printf("Name:%s\nLevel:%d\n",current->name,current->level);
        current = current->players;
    }
}

void push(player_t * player,char * name,int level){
    player_t * current = player;

    while (current->players != NULL) {
        current = current->players;
    }

    current->players = (player_t*)malloc(sizeof(player_t));
    current->players->level = level;
    current->players->name = name;
    current->players->players = NULL;
}

int remove_lazt(player_t * player){
    if (player->players == NULL){
        free(player);
        return 0;
    }

    player_t * current = player;

    while (current->players->players != NULL) {
        current = current->players;
    }

    free(current->players);
    current->players = NULL;
    return 0;
}

int main(){
    //Init Data
    player_t * player = NULL;
    player = (player_t *)malloc(sizeof(player_t));

    if(player == NULL){
        return 1;
    }
    
    player->name = "Humanz";
    player->level = 10;
    player->players =NULL;
    
    push(player, "Kano", 30);

    //Should print Humanz then Kano
    print_player(player);

    remove_lazt(player);

    //Should print Humanz
    print_player(player);
    return 0;
}