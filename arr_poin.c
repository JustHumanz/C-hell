#include <stdio.h>
#include <stdlib.h>

int main(){
int arr1 = 2;
int arr2 = 6;
int i,ii;

char **humanz = (char **)malloc(arr1 * sizeof(char *));

humanz[0] = (char *) malloc(arr2 * sizeof(char *));
humanz[1] = (char *) malloc(arr2 * sizeof(char *));

humanz[0][0] = 'H';
humanz[0][1] = 'U';
humanz[0][2] = 'M';
humanz[0][3] = 'A';
humanz[0][4] = 'N';
humanz[0][5] = 'Z';

humanz[1][0] = 'z';
humanz[1][1] = 'n';
humanz[1][2] = 'a';
humanz[1][3] = 'm';
humanz[1][4] = 'u';
humanz[1][5] = 'h';

for (i=0; i < arr1; i++) {
    for (ii=0; ii < arr2; ii++) {
        printf("%c",humanz[i][ii]);
    }
    printf("\n");
}

free(humanz[0]);
free(humanz[1]);

free(humanz);
}

