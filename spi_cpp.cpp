#include "Bitcraze_PMW3901.h"
#include <bcm2835.h>

int main(){

    Bitcraze_PMW3901 obj(0);
    if(obj.begin() == 0)
       { printf("init failed\n");}
    else
       { printf("init ok\n");}

   for(int i = 0; i < 20;i++)
   {
      obj.readMotionCount();
      printf("x:%d   y:%d\n",obj.getX(), obj.getY());
      delay(1000);
   }

    return 0;
}