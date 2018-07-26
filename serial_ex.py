import serial
import time
import sys
import threading


SERIAL_PORT_IN = "\\.\\COM"+ sys.argv[1]
serial_stream = None

BAUDRATE = 115200

serial_lock = threading.Lock()

brew_request = \
    '"ID":1\n"USR":1\n"TIME":1\n"CMD":"BREW"\n"H2O_TEMP":3\n"EXEC_TIME":0\n"H2O_AMOUNT":100\n\0'

state_request = \
    '"ID":1\n"USR":1\n"TIME":1\n"CMD":"STATE"\n\0'

keepwarm_request = \
    '"ID":1\n"USR":1\n"TIME":1\n"CMD":"BREW"\n"DURATION":10\n\0'



requests = [brew_request, state_request, keepwarm_request]

def sync_serial_read():
    serial_lock.acquire()
    print( serial_stream.readline().decode('ascii') )
    serial_lock.release()

def sync_serial_write( data ):
    serial_lock.acquire()
    serial_stream.write(bytes(data,'ascii'))
    serial_stream.flush()
    serial_lock.release()

if __name__ == "__main__":
	# open the connection

	serial_stream = serial.Serial(port=SERIAL_PORT_IN, baudrate=BAUDRATE, 
		parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, 
		bytesize=serial.EIGHTBITS, timeout=50)

	counter = 0

	while True:
		sync_serial_write( requests[counter] )
		counter = (counter + 1) % len(requests)
		sync_serial_read()
		sync_serial_read()
		time.sleep(2)
		
