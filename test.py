import serial
from time import sleep

ser=serial.Serial(port="/dev/cu.usbserial-1410",baudrate=9600)

sleep(2)

a='100'


ser.write(a.encode('utf-8'))

while(1):
    data = ser.readline().decode('utf-8').rstrip() #接收数据
    print(data)
    ser.write(data.encode('utf-8'))