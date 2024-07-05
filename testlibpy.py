from threading import Thread
from time import sleep
from ctypes import cdll
from ctypes import *

MPU = cdll.LoadLibrary('./libMPU6050.so')
MPU.get_yarn.restype = c_float
MPU.get_pitch.restype = c_float
MPU.get_roll.restype = c_float
MPU.get_AccX.restype = c_float
MPU.get_AccY.restype = c_float
MPU.get_AccZ.restype = c_float
MPU.get_GyroX.restype = c_float
MPU.get_GyroY.restype = c_float
MPU.get_GyroZ.restype = c_float

def func():
	MPU.init_MPU()

th = Thread(target=func)
th.start()
sleep(1.2)
while 1:
 MPU.print_all()
 sleep(0.1)

th.join()
