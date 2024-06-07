/*
I2Cdev library collection - MPU6050 RPi example
Based on the example in Arduino/MPU6050/

==============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================

To compile on a Raspberry Pi (1 or 2)
  1. install the bcm2835 library, see http://www.airspayce.com/mikem/bcm2835/index.html
  2. enable i2c on your RPi , see https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c
  3. connect your i2c devices
  4. then from bash
      $ PATH_I2CDEVLIB=~/i2cdevlib/
      $ gcc -o MPU6050_example_1 ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/MPU6050/examples/MPU6050_example_1.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/MPU6050/ ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/MPU6050/MPU6050.cpp -l bcm2835 -l m
      $ sudo ./MPU6050_example_1

*/

#include <stdio.h>
#include <bcm2835.h>
#include "I2Cdev.h"
#include "MPU6050.h"
//#include "MPU6050_6Axis_MotionApps20.h"
#include <math.h>

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

int main(int argc, char **argv) {
  printf("MPU6050 3-axis acceleromter example program\n");
  I2Cdev::initialize();
  MPU6050 accelgyro ;
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  if ( accelgyro.testConnection() ) 
    printf("MPU6050 connection test successful\n") ;
  else {
    fprintf( stderr, "MPU6050 connection test failed! something maybe wrong, continuing anyway though ...\n");
    //return 1;
  }
  accelgyro.initialize();
  // use the code below to change accel/gyro offset values
  /*
  printf("Updating internal sensor offsets...\n");
  // -76	-2359	1688	0	0	0
  printf("%i \t %i \t %i \t %i \t %i \t %i\n", 
	 accelgyro.getXAccelOffset(),
	 accelgyro.getYAccelOffset(),
	 accelgyro.getZAccelOffset(),
	 accelgyro.getXGyroOffset(),
	 accelgyro.getYGyroOffset(),
	 accelgyro.getZGyroOffset());
  accelgyro.setXGyroOffset(220);
  accelgyro.setYGyroOffset(76);
  accelgyro.setZGyroOffset(-85);
  printf("%i \t %i \t %i \t %i \t %i \t %i\n", 
	 accelgyro.getXAccelOffset(),
	 accelgyro.getYAccelOffset(),
	 accelgyro.getZAccelOffset(),
	 accelgyro.getXGyroOffset(),
	 accelgyro.getYGyroOffset(),
	 accelgyro.getZGyroOffset());
  */
  
  printf("\n");
  printf("  ax \t ay \t az \t gx \t gy \t gz:\n");
  while (true) {
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    printf("  %d \t %d \t %d \t %d \t %d \t %d\r", ax, ay, az, gx, gy, gz);
    fflush(stdout);
    bcm2835_delay(100);
  }
  return 1; 
}
