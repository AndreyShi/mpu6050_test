import socket
from time import sleep
from ctypes import cdll
from ctypes import *
from threading import Thread
import struct
#import dalnomer

MPU = cdll.LoadLibrary('/home/pi/mpu-setup/libMPU6050.so')
MPU.get_yarn.restype = c_float
MPU.get_pitch.restype = c_float
MPU.get_roll.restype = c_float
MPU.get_AccX.restype = c_float
MPU.get_AccY.restype = c_float
MPU.get_AccZ.restype = c_float
MPU.get_GyroX.restype = c_float
MPU.get_GyroY.restype = c_float
MPU.get_GyroZ.restype = c_float

th = Thread(target=MPU.init_MPU)
th.start()
sleep(1.2)

#th2 = Thread(target=dalnomer.poll)
#th2.start()
#sleep(2)

bufferSize=1024
msgFromServer="I'am Server"
ServerPort=2222
ServerIP='172.16.0.1'
bytesToSend=msgFromServer.encode('utf-8')
RPIsocket=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
RPIsocket.bind((ServerIP,ServerPort))
print('Server is Up and Listening...')
while 1:
 message,address=RPIsocket.recvfrom(bufferSize)
 message=message.decode('utf-8')
 print(message)
 print('Client Address',address[0])
 if message == 'give_MPU_bin_data':
  MPU_data = (MPU.get_yarn(),MPU.get_pitch(),MPU.get_roll(),MPU.get_AccX(),MPU.get_AccY(),MPU.get_AccZ(),MPU.get_GyroX(),MPU.get_GyroY(),MPU.get_GyroZ())
  print(MPU_data)
  bytes_obj = struct.pack('!fffffffff',*MPU_data)
  RPIsocket.sendto(bytes_obj,address)
 elif message == 'get_distance':
  bytes_obj = bytes(dalnomer.distance,'utf-8')
  RPIsocket.sendto(bytes_obj,address)
 else:
  RPIsocket.sendto(bytesToSend,address)
