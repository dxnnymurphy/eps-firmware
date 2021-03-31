#This code uses UART communication to loop back information sent to the pi by the msp430,
#used to test the functionality of the UART driver

import serial
from time import sleep

ser = serial.Serial ("/dev/ttyS0", 9600) #open port and set baud rate
while True:
    received_data = ser.read() #read serial port
    sleep(0.03)
    data_left = ser.inWaiting() #check for remaining bytes
    received_data +=ser.read(data_left) 
    print(received_data) #print received data
    ser.write(received_data) #transmit data back serially