#include <stdio.h>
#include <stdlib.h>

typedef struct player{
    char * name;
    int level;
    struct player * next;
}player_t;

void print_list(player_t * player) {
    player_t * current = player;
    while (current != NULL) {
        printf("User:%s\nLevel:%d\n",current->name,current->level);
        current = current->next;
    }
}

void push(player_t * user,char * name,int level) {
    player_t * current_user = user;
    while (current_user->next != NULL) {
        current_user = current_user->next;
    }

    current_user->next = (player_t *)malloc(sizeof(player_t));
    current_user->name = name;
    current_user->level = level;
    current_user->next = NULL;
}


int main(){
    player_t * user = NULL;
    user = (player_t *)malloc(sizeof(player_t));
    if (user == NULL){
        return 1;
    }

    user->level = 10;
    user->name = "Humanz";

    print_list(user);

    printf("================PUSH================\n");
    push(user,"Kano",20);

    print_list(user);
    return 0;
}