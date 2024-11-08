#include "Bitcraze_PMW3901.h"
#include <bcm2835.h>

int main(){

    Bitcraze_PMW3901 obj(0);
    if(obj.begin() == 0)
       { printf("init failed\n");}
    else
       { printf("init ok\n");}
   int16_t deltaX = 0;
   int16_t deltaY = 0;

   for(int i = 0; i < 20;i++)
   {
      obj.readMotionCount(&deltaX, &deltaY);
      printf("x:%d   y:%d\n",deltaX, &deltaY);
      delay(1000);
   }

    return 0;
}