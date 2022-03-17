#!/bin/python3
import sys
import serial
import time

if(len(sys.argv) != 2):
    print("Usage: ./uartboot.py <serial>")
    exit(1)

port = '/dev/pts/' + sys.argv[1]
# port = '/dev/ttyUSB0'
baudrate = 115200

ser = serial.Serial(port, baudrate)
with open('kernel8.img', 'rb') as f:
    data = f.read()
    kernel_size = str(len(data)) + '\n'
    print(f'kernel size: {kernel_size}')
    ser.write(kernel_size.encode())
    # ser.write(kernel_size.encode().ljust(10, b'\0'))
    ser.flush()
    time.sleep(0.5)
    
    for i in range(len(data)):
        ser.write(data[i:i+1])
        ser.flush()
        # time.sleep(0.001)