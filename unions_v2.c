#include <stdio.h>

struct operator {
    int type;
    union {
      int intNum;
      float floatNum;
      double doubleNum;
      char chrNum;
    } types;
};

int main(){
    struct operator humanz;
    humanz.type = 0;
    humanz.types.intNum = 90;

    printf("type: %d\nintNum: %d\nfloatNum: %.6f\ndoubleNum: %f\nchrNum: %c",humanz.type,humanz.types.intNum,humanz.types.floatNum,humanz.types.doubleNum,humanz.types.chrNum);
    return 0;
}