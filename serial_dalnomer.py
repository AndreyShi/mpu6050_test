import serial

ser = serial.Serial('/dev/ttyS0', 115200, timeout=1)
#ser.open()
if ser.isOpen():
    print('Serial connection established!')
    ser.write(str.encode('<MAcont>'))
else:
    print('Error: Failed to establish serial connection.')
ser.close()
