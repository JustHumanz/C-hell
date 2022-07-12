#include <stdio.h>

union uni_test {
  int theInt;
  char theChr;
};

int main(){
    union uni_test humanz;
    humanz.theInt = 90;

    //Should print 90 and Z
    printf("the Int : %d\nthe Chr : %c",humanz.theInt,humanz.theChr);
    return 0;
}