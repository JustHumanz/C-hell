#include <stdio.h>
#include <stdlib.h>

int compare(const void* a,const void* b){
    return (*(int*)a - *(int*)b);
}

int main(){
    int (*cmp)(const void*,const void*);

    cmp = &compare;
    int iarray[] = {1,2,3,4,5,6,7,8,9};
    qsort(iarray, sizeof(iarray)/sizeof(*iarray), sizeof(*iarray), cmp);

    int c = 0;
    while (c < sizeof(iarray)/sizeof(*iarray))
    {
        printf("%d \t", iarray[c]);
        c++;
    }    
}