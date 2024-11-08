#include "Bitcraze_PMW3901.h"
#include <bcm2835.h>

int main(){

    if(Bitcraze_PMW3901_begin() == 0)
       { printf("init failed\n");}
    else
       { printf("init ok\n");}

   for(int i = 0; i < 20;i++)
   {
      Bitcraze_PMW3901_readMotionCount();
      printf("x:%d   y:%d\n",Bitcraze_PMW3901_getX(), Bitcraze_PMW3901_getY());
      delay(1000);
   }

    return 0;
}