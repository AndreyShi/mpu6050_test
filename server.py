import socket
from time import sleep
from ctypes import cdll
from ctypes import *
from threading import Thread
import struct
import dalnomer
import math

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
DAL = cdll.LoadLibrary('./libPMW3901.so')
DAL.Bitcraze_PMW3901_begin.restype = c_bool
DAL.Bitcraze_PMW3901_getX.restype = c_int16
DAL.Bitcraze_PMW3901_getY.restype = c_int16

th = Thread(target=MPU.init_MPU)
th.start()
sleep(1.2)

th2 = Thread(target=dalnomer.poll)
th2.start()
sleep(2)

#потоковая функция
delX = 0
delY = 0
def optic_flow():
 DAL.Bitcraze_PMW3901_begin()# инициализация датчика оптического потока
 while 1:
  #  получение дельты от оптического потока
  DAL.Bitcraze_PMW3901_readMotionCount()
  delX += abs(int(DAL.Bitcraze_PMW3901_getX()))
  delY += abs(int(DAL.Bitcraze_PMW3901_getY()))
  #print('dalnomer.distance:',dalnomer.distance,'delX:',delX,'delY:',delY)
  dist_f = float(dalnomer.distance)# сконвертировать dalnomer.distance во float
  #Если один из концов отрезка совпадает с началом координат, а другой имеет координаты М(хМ; уМ), то формула для вычисления d примет вид ОМ = √(хМ2 + уМ2).
  #Источник: https://blog.tutoronline.ru/rasstojanie-mezhdu-dvumja-tochkami-na-ploskosti
  dis_fl = math.sqrt(delX**2 + delY**2)
  '''
  высота H=10м
  дельта к примеру D=50 юнитов (в чем у них там вывод)
  соответствует реальной длине L=2м
  соответственно коэффициент будет:
  K=(2/50)/10 = 0.004 - это количество метров на 1 юнит дельты на высоте 1м
  '''
  K=(2/50)/dist_f
  real_distance = dis_fl * K
  print('real_distance:',real_distance)
  sleep(1)#  пауза от 100 мс (пока непонятны ограничения с какой частотой надо опрашивать датчик оптического потока)
th3 = Thread(target=optic_flow)
th3.start()
sleep(1)

bufferSize=1024
msgFromServer="I'am Server"
ServerPort=2222
ServerIP='172.16.0.1'# it's host wlan ip
bytesToSend=msgFromServer.encode('utf-8')
RPIsocket=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
RPIsocket.bind((ServerIP,ServerPort))
print('Server is Up and Listening...')
while 1:
 message,address=RPIsocket.recvfrom(bufferSize) #blocking
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
 elif message == 'get_flow_delta':
  bytes_obj = bytes(delX,'utf-8')
  RPIsocket.sendto(bytes_obj,address)
 else:
  RPIsocket.sendto(bytesToSend,address)
