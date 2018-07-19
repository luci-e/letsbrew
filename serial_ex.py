import serial
import time
import sys


SERIAL_PORT_IN = "\\.\\COM"+ sys.argv[1]
SERIAL_PORT_OUT = "\\.\\COM"+ sys.argv[2]

BAUDRATE = 9600
counter = 512


if __name__ == "__main__":
	# open the connection

	ser_in = serial.Serial(port=SERIAL_PORT_IN, baudrate=BAUDRATE, 
		parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, 
		bytesize=serial.EIGHTBITS, timeout=0)

	ser_out = serial.Serial(port=SERIAL_PORT_OUT, baudrate=BAUDRATE, 
		parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, 
		bytesize=serial.EIGHTBITS, timeout=500)

	while True:
		#line = input('>')
		ser_out.write(bytes('hello\r\n', encoding='ascii'))

		echo = ser_in.readline()
		print(echo)

	