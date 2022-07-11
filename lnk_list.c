#include <stdio.h>
#include <stdlib.h>

typedef struct player{
    char * name;
    int level;
    struct player * next;
}player_t;


int main(){
    player_t * user = NULL;
    user = (player_t *)malloc(sizeof(player_t));
    if (user == NULL){
        return 1;
    }

    user->level = 10;
    user->name = "Humanz";
    user->next = (player_t *)malloc(sizeof(player_t));
    user->next->level = 20;
    user->next->name = "Kano";
    user->next->next = NULL;

    player_t * current = user;
    while (current != NULL) {
        printf("User :%s\nLevel:%d\n",current->name,current->level);
        current = current->next;
    }

    return 0;
}