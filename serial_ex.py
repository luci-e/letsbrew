import serial
import time
import sys


SERIAL_PORT_IN = "\\.\\COM"+ sys.argv[1]

BAUDRATE = 115200
counter = 512


if __name__ == "__main__":
	# open the connection

	ser_in = serial.Serial(port=SERIAL_PORT_IN, baudrate=BAUDRATE, 
		parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, 
		bytesize=serial.EIGHTBITS, timeout=300)

	while True:
		echo = ser_in.readline()
		print(echo)

	