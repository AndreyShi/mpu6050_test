from threading import Thread
from time import sleep
from ctypes import cdll

MPU = cdll.LoadLibrary('./libMPU6050.so')

def func():
	MPU.init_MPU()

th = Thread(target=func)
th.start()
sleep(1.2)
while 1:
 MPU.print_all()
 sleep(0.1)

th.join()
