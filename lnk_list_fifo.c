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

void push(player_t * player,char *name,int level){
    player_t * current = player;
    while (current->players != NULL) {
        current = current->players;
    }

    current->players = (player_t *)malloc(sizeof(player_t));
    current->players->name = name;
    current->players->level = level;
    current->players->players = NULL;
}

int pop(player_t ** player) {
    char * retname;
    int retlvl = -1;
    player_t * del_players = NULL;

    if (*player == NULL) {
        return 1;
    }

    del_players = (*player)->players;
    retlvl = (*player)->level;
    retname = (*player)->name;

    free(*player);
    *player = del_players;
    return retlvl;
    
}
int main(){
    //Init data
    player_t * player = NULL;
    player = (player_t *)malloc(sizeof(player_t));

    if (player == NULL){
        return 1;
    } 

    player->name = "Humanz";
    player->level = 10;
    player->players = NULL;

    //Add new data 
    push(player,"Kano",20);

    //Should print Humanz first then Kano 
    print_player(player);

    //Remove Humanz from list
    pop(&player);

    //Should print Kano only
    print_player(player);
    return 0;
}