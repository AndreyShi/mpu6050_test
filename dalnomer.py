import serial
import re

distance = 'measurment distance from device'
def poll():
 ser = serial.Serial('/dev/ttyS0', 115200, timeout=2)
 #ser.open()
 if ser.isOpen():
    print('Serial connection established!')
    ser.write(str.encode('<MAcont>'))
    while 1:
        temp = ser.read(10) #1200.00m  
        result = re.search("\d+.\d\dm",temp)
        distance = result.group(0)
 else:
    print('Error: Failed to establish serial connection.')
 ser.close()

poll()
