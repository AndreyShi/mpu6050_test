from threading import Thread
from time import sleep
from ctypes import cdll
from ctypes import *

DAL = cdll.LoadLibrary('./libPMW3901.so')
DAL.Bitcraze_PMW3901_begin.restype = c_bool
DAL.Bitcraze_PMW3901_getX.restype = c_int16
DAL.Bitcraze_PMW3901_getY.restype = c_int16


print("Test call function \"testPy\" from lib")
DAL.testPy()
for _ in range(30):
    DAL.readFlow()
    print('X:',DAL.Bitcraze_PMW3901_getX())
    print('Y:',DAL.Bitcraze_PMW3901_getY())
    sleep(0.1)
#DAL.Bitcraze_PMW3901_begin()
#DAL.Bitcraze_PMW3901_readMotionCount()