/*
	MPU6050 Interfacing with Raspberry Pi
	http://www.electronicwings.com
*/

#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <tgmath.h>

#define Device_Address 0x68	/*Device Address/Identifier for MPU6050*/

#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define INT_ENABLE   0x38
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H  0x43
#define GYRO_YOUT_H  0x45
#define GYRO_ZOUT_H  0x47

#define RAD_2_DEG             57.29578 // [deg/rad]
#define CALIB_OFFSET_NB_MES   500
#define TEMP_LSB_2_DEGREE     340.0    // [bit/celsius]
#define TEMP_LSB_OFFSET       12412.0

#define DEFAULT_GYRO_COEFF    0.98

int fd;

void MPU6050_Init(){
	
	wiringPiI2CWriteReg8 (fd, SMPLRT_DIV, 0x07);	/* Write to sample rate register */
	wiringPiI2CWriteReg8 (fd, PWR_MGMT_1, 0x01);	/* Write to power management register */
	wiringPiI2CWriteReg8 (fd, CONFIG, 0);		/* Write to Configuration register */
	wiringPiI2CWriteReg8 (fd, GYRO_CONFIG,0x08);	/* Write to Gyro Configuration register */
	wiringPiI2CWriteReg8 (fd, INT_ENABLE, 0x01);	/*Write to interrupt enable register */

	} 
short read_raw_data(int addr){
	short high_byte,low_byte,value;
	high_byte = wiringPiI2CReadReg8(fd, addr);
	low_byte = wiringPiI2CReadReg8(fd, addr+1);
	value = (high_byte << 8) | low_byte;
	return value;
}

void ms_delay(int val){
	int i,j;
	for(i=0;i<=val;i++)
		for(j=0;j<1200;j++);
}

/* Wrap an angle in the range [-limit,+limit] (special thanks to Edgar Bonet!) */
static float wrap(float angle,float limit){
  while (angle >  limit) angle -= 2*limit;
  while (angle < -limit) angle += 2*limit;
  return angle;
}
void SetConsolePos(int x,int y)
{
	printf("\33[%d;%dH", x ,y);
}

int main(){
	
	float Acc_x,Acc_y,Acc_z;
	float Gyro_x,Gyro_y,Gyro_z;
	float accX=0, accY=0, accZ=0;
	float gyroX=0, gyroY=0, gyroZ=0;
    float angleAccX, angleAccY;
    float angleX = 0, angleY = 0, angleZ = 0;

    float filterGyroCoef = DEFAULT_GYRO_COEFF; // complementary filter coefficient to balance gyro vs accelero data to get angle

    unsigned long preInterval;

	fd = wiringPiI2CSetup(Device_Address);   /*Initializes I2C with device Address*/
	MPU6050_Init();		                 /* Initializes MPU6050 */
        preInterval = millis(); // may cause lack of angular accuracy if begin() is much before the first update()

	for(int i = 0 ; i < 20; i++)
	{
		/*Read raw value of Accelerometer and gyroscope from MPU6050*/
		Acc_x = read_raw_data(ACCEL_XOUT_H);
		Acc_y = read_raw_data(ACCEL_YOUT_H);
		Acc_z = read_raw_data(ACCEL_ZOUT_H);
		
		Gyro_x = read_raw_data(GYRO_XOUT_H);
		Gyro_y = read_raw_data(GYRO_YOUT_H);
		Gyro_z = read_raw_data(GYRO_ZOUT_H);
		
		/* Divide raw value by sensitivity scale factor */
		accX = Acc_x/16384.0;
		accY = Acc_y/16384.0;
		accZ = Acc_z/16384.0;
		
		gyroX = Gyro_x/65.5;
		gyroY = Gyro_y/65.5;
		gyroZ = Gyro_z/65.5;
		
		printf("\n Gx=%.3f °/s\tGy=%.3f °/s\tGz=%.3f °/s\taX=%.3f g\taY=%.3f g\taZ=%.3f g\n",gyroX,gyroY,gyroZ,accX,accY,accZ);


        float sgZ = accZ<0 ? -1 : 1; // allow one angle to go from -180 to +180 degrees
        angleAccX =   atan2(accY, sgZ*sqrt(accZ*accZ + accX*accX)) * RAD_2_DEG; // [-180,+180] deg
        angleAccY = - atan2(accX,     sqrt(accZ*accZ + accY*accY)) * RAD_2_DEG; // [- 90,+ 90] deg

        unsigned long Tnew = millis();
        float dt = (Tnew - preInterval) * 1e-3;
        preInterval = Tnew;

        // Correctly wrap X and Y angles (special thanks to Edgar Bonet!)
        // https://github.com/gabriel-milan/TinyMPU6050/issues/6
        angleX = wrap(filterGyroCoef*(angleAccX + wrap(angleX +     gyroX*dt - angleAccX,180)) + (1.0-filterGyroCoef)*angleAccX,180);
        angleY = wrap(filterGyroCoef*(angleAccY + wrap(angleY + sgZ*gyroY*dt - angleAccY, 90)) + (1.0-filterGyroCoef)*angleAccY, 90);
        angleZ += gyroZ*dt; // not wrapped

		printf("\n angleX=%.3f °\tangleY=%.3f °\tangleZ=%.3f",angleX,angleY,angleZ);
		SetConsolePos(0,0);
		delay(10);
		
	}
	return 0;
}
