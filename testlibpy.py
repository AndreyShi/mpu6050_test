from threading import Thread
from time import sleep
from ctypes import cdll

MPU = cdll.LoadLibrary('./libMPU6050.so')

def func():
	MPU.init_MPU()

th = Thread(target=func)
th.start()

while 1:
 MPU.print_all()
 sleep(0.5)

th.join()
