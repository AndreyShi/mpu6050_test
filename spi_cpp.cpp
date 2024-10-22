#include "Bitcraze_PMW3901.h"

int main(){

    Bitcraze_PMW3901 obj(0);
    if(obj.begin() == 0)
       { printf("init failed");}
    else
       { printf("init ok");}

    return 0;
}