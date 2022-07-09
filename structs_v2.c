#include <stdio.h>
#include <string.h>

struct humanz {
    char a[10];
    int b;
};

struct just {
    int code;
    struct humanz payload;
};

int main(){
    struct just JustHumanz;
    JustHumanz.code = 404;
    strcpy(JustHumanz.payload.a,"Humanz");
    JustHumanz.payload.b = 403;

    printf("%d \t %s \t %d\n",JustHumanz.code,JustHumanz.payload.a,JustHumanz.payload.b);
    return 0;
}