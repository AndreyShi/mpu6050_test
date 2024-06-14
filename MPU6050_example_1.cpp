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
//#include "MPU6050.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <math.h>
#include "MPU6050lib.h"

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
  MPU6050_Base mpu ;
  int16_t ax, ay, az;
  int16_t gxyz[3];//, gy, gz;
// Input on RPi pin GPIO 15
#define PIN RPI_GPIO_P1_15

float get_yarn(void)
{return ypr[0] * 180/M_PI;}
float get_pitch(void)
{return ypr[1] * 180/M_PI;}
float get_roll(void)
{return ypr[2] * 180/M_PI;}

float get_AccX(void)
{return (float)aaReal.x/16384.0;}
float get_AccY(void)
{return (float)aaReal.y/16384.0;}
float get_AccZ(void)
{return (float)aaReal.z/16384.0;}

float get_GyroX(void)
{return (float)gxyz[0]/16.4;}
float get_GyroY(void)
{return (float)gxyz[1]/16.4;}
float get_GyroZ(void)
{return (float)gxyz[2]/16.4;}

//int main(int argc, char **argv) {
int main_MPU(void) {
    if (!bcm2835_init())
        return 1; 
    // Set RPI pin P1-15 to be an input
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
    //  with a pullup
    bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_DOWN);
  printf("MPU6050 3-axis acceleromter example program\n");
  I2Cdev::initialize();

  while(1){
      bool tst = mpu.testConnection();
      Serial.println(tst ? "MPU6050 connection successful" : "MPU6050 connection failed");
      if(tst == false)
          { bcm2835_delay(5000);}
      else
          { break;}
  }
  mpu.initialize();

  #ifndef DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();
  if (devStatus == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);//old 6
        mpu.CalibrateGyro(6);//old 6
        
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        //Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));// we set it on hal drivers
        Serial.println(F(")..."));
        //attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);// we set it on hal drivers
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
  #endif

    while(1){
        mpu.resetFIFO();
        bcm2835_delay(100);

        if (!dmpReady) {
            printf("dmp is not ready!\n");
            continue;
        }
        // read a packet from FIFO
        mpuIntStatus = mpu.getIntStatus();
        // get current FIFO count
        fifoCount = mpu.getFIFOCount();

        printf("Int: %d  Cnt:%d  ",mpuIntStatus, fifoCount);

        // check for overflow (this should never happen unless our code is too inefficient)
        if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
          // reset so we can continue cleanly
          mpu.resetFIFO();
          Serial.println(F("FIFO overflow!"));

          // otherwise, check for DMP data ready interrupt (this should happen frequently)
        } else if (mpuIntStatus & 0x02) {
          // wait for correct available data length, should be a VERY short wait
          if(fifoCount < packetSize || fifoCount % packetSize) 
          {
            printf("bad packet, reset FIFO...!\n");
            continue;
          }
          // read a packet from FIFO
          if(mpu.getFIFOBytes(fifoBuffer, packetSize) == 0)
          {
            printf("read fifo problem..!\n");
            continue;
          }

          fifoCount -= packetSize;
          mpu.dmpGetQuaternion(&q, fifoBuffer);
          mpu.dmpGetGravity(&gravity, &q);
          mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
          //printf("%2ld:%2ld:%3ld  ", timer1m,timer1s,timer1ms);
          printf("ypr  %.3f %.3f %.3f", ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);

          //AFS_SEL Full Scale Range LSB Sensitivity 0 ±2g 16384 LSB/g, 1 ±4g 8192 LSB/g, 2 ±8g 4096 LSB/g, 3 ±16g 2048 LSB/g
          mpu.dmpGetAccel(&aa, fifoBuffer);//for linear
          mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);//for linear          
          printf("  acc %.3f %.3f %.3f  ",(float)aaReal.x/16384.0,(float)aaReal.y/16384.0,(float)aaReal.z/16384.0);// todo view LinearAccel on display  

          //FS_SEL Full Scale Range LSB Sensitivity 0 ±250 °/s 131 LSB/°/s, 1 ±500 °/s65.5 LSB/°/s, 2 ±1000 °/s 32.8 LSB/°/s, 3 ±2000 °/s 16.4 LSB/°/s
          mpu.dmpGetGyro(gxyz, fifoBuffer);//for uglova9 speed
          printf("spd %.3f %.3f %.3f",(float)gxyz[0]/16.4,(float)gxyz[1]/16.4,(float)gxyz[2]/16.4);//todo view uglova9 speed on display  
          printf("\n");

        } else if (mpuIntStatus == 1 && fifoCount == 0) {
            printf("resetting DMP...\n");
            mpu.resetDMP();
            mpu.setDMPEnabled(false);
            delay(50);
            mpu.setDMPEnabled(true);
        } else
              { ;}
  }
  return 1; 
}
