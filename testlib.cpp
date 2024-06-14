#include <bcm2835.h>
#include <pthread.h>
#include <cstdio>
#include "MPU6050lib.h"


void* threadFunc(void* thread_data){
	init_MPU();
}

int main(int argc, char **argv) {
   void* thread_data = NULL;
   pthread_t thread;
 
   pthread_create(&thread, NULL, threadFunc, thread_data);
   while(1){
        delay(1000);
        printf("main thread\n");
   }
   pthread_join(thread, NULL);
   return 0;
}
