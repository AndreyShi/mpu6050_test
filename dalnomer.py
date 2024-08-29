import serial

distance = 'measurment distance from device'
def poll():
 ser = serial.Serial('/dev/ttyS0', 115200, timeout=2)
 #ser.open()
 if ser.isOpen():
    print('Serial connection established!')
    ser.write(str.encode('<MAcont>'))
    while 1:
        distance = ser.read()
 else:
    print('Error: Failed to establish serial connection.')
 ser.close()

poll()
