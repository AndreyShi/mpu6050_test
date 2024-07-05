import socket
msgFromClient = 'give_MPU_data'
bytesToSend = msgFromClient.encode('utf-8')
serverAddress=('192.168.0.100',2222)
bufferSize=1024
UDPClient=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
UDPClient.sendto(bytesToSend,serverAddress)
data,address=UDPClient.recvfrom(bufferSize)
data=data.decode('utf-8')
print('Data from server: ',data)
print('Server IP Addres: ',address[0])
print('Server Port: ',address[1])